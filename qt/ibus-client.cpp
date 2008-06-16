#include <QtDebug>
#include <QFile>
#include <QDBusConnection>
#include <QCoreApplication>
#include <QDBusMessage>

#include "ibus-client.h"
#include "ibus-input-context.h"

#ifdef Q_WS_X11
# include <QX11Info>
# include <X11/Xlib.h>
# include <X11/keysym.h>
# include <X11/Xutil.h>
#endif

#define IBUS_NAME	"org.freedesktop.IBus"
#define IBUS_PATH	"/org/freedesktop/IBus"
#define IBUS_INTERFACE	"org.freedesktop.IBus"


IBusClient::IBusClient ()
	: ibus (NULL)
{
	connectToBus ();

	QObject::connect (
		&watcher,
		SIGNAL(directoryChanged(const QString &)),
		this,
		SLOT(slotDirectoryChanged(const QString &)));
	QString ibus_dir;
	ibus_dir = QString ("/tmp/ibus-%1/").arg (getenv ("USER"));
	watcher.addPath (ibus_dir);
}

IBusClient::~IBusClient ()
{
	if (ibus)
		delete ibus;
}

QString
IBusClient::createInputContextRemote ()
{
	QString ic;
	if (ibus) {
		QDBusMessage message = QDBusMessage::createMethodCall (
								IBUS_NAME,
								IBUS_PATH,
								IBUS_INTERFACE,
								"CreateInputContext");
		message << QCoreApplication::applicationName ();
		message = ibus->call (message);

		if (message.type () == QDBusMessage::ErrorMessage) {
			qWarning() << message.errorMessage ();
		}
		else if (message.type () == QDBusMessage::ReplyMessage) {
			ic = message.arguments () [0].toString ();
		}
	}
	return ic;
}

QInputContext *
IBusClient::createInputContext ()
{
	IBusInputContext *ctx;
	QString ic;

	ic = createInputContextRemote ();

	ctx = new IBusInputContext (0, this, ic);
	context_list.append (ctx);

	if (! ic.isEmpty ()) {
		context_dict[ic] = ctx;
	}

	return (QInputContext *) ctx;
}

void
IBusClient::releaseInputContext (IBusInputContext *ctx)
{
	Q_ASSERT (ctx);

	QString ic = ctx->getIC ();

	if (ibus && !ic.isEmpty ()) {
		QDBusMessage message = QDBusMessage::createMethodCall (
								IBUS_NAME,
								IBUS_PATH,
								IBUS_INTERFACE,
								"ReleaseInputContext");
		message << ctx->getIC ();
		message = ibus->call (message);

		if (message.type () == QDBusMessage::ErrorMessage) {
			qWarning() << message.errorMessage ();
		}
		context_dict.remove (ic);
	}
	context_list.removeAll (ctx);
}

#ifndef Q_WS_X11
static void
translate_key_event (const QKeyEvent *event, quint32 *keyval, bool *is_press, quint32 *state)
{
	Q_ASSERT (event);
	Q_ASSERT (keyval);
	Q_ASSERT (is_press);
	Q_ASSERT (state);

	*keyval = event->key ();
	*is_press = (event->type() == QEvent::KeyPress);

	Qt::KeyboardModifiers modifiers = event->modifiers ();
	*state = 0;
	if (modifiers & Qt::ShiftModifier) {
		*state |= (1<< 0);
	}
	if (modifiers & Qt::ControlModifier) {
		*state |= (1<< 2);
	}
	if (modifiers & Qt::AltModifier) {
		*state |= (1<< 3);
	}
	if (modifiers & Qt::MetaModifier) {
		*state |= (1<< 28);
	}
	if (modifiers & Qt::KeypadModifier) {
		// *state |= (1<< 28);
	}
	if (modifiers & Qt::GroupSwitchModifier) {
		// *state |= (1<< 28);
	}
}

bool
IBusClient::filterEvent (IBusInputContext *ctx, QEvent *event)
{
	return true;
}
#endif

#ifdef Q_WS_X11
static inline bool
translate_x_key_event (XEvent *xevent, quint32 *keyval, quint32 *state, bool *is_press)
{
	Q_ASSERT (xevent);
	Q_ASSERT (keyval);
	Q_ASSERT (state);
	Q_ASSERT (is_press);

	if (xevent->type != KeyPress && xevent->type != KeyRelease)
		return false;

	*is_press = (xevent->type == KeyPress);
	*state = xevent->xkey.state;

	char key_str[64];

	if (XLookupString (&xevent->xkey, key_str, sizeof (key_str), (KeySym *)keyval, 0) <= 0) {

	}

	return true;

}

bool
IBusClient::x11FilterEvent (IBusInputContext *ctx, QWidget *keywidget, XEvent *xevent)
{
	quint32 keyval;
	quint32 state;
	bool is_press;

	if (ibus == NULL || !ibus->isConnected ())
		return false;

	if (!translate_x_key_event (xevent, &keyval, &state, &is_press))
		return false;

	qDebug ("ic = %p: %c 0x%08x %d", ctx, keyval, state, is_press);

	return true;
}
#endif

bool
IBusClient::connectToBus ()
{
	QString address;
	QString session;
	QDBusConnection *connection = NULL;

	if (ibus != NULL)
		return false;

	session = getenv ("DISPLAY");
	session.replace (":", "-");
	address = QString("unix:path=/tmp/ibus-%1/ibus-%2").arg (getenv ("USER"), session);

	connection = new QDBusConnection (
		QDBusConnection::connectToBus (
			address,
			QString ("ibus")));

	if (!connection->isConnected ()) {
		delete connection;
		QDBusConnection::disconnectFromBus ("ibus");
		return false;
	}

	if (!connection->connect ("",
			"",
			"org.freedesktop.DBus.Local",
			"Disconnected",
			this, SLOT (slotIBusDisconnected()))) {
		qWarning () << "Can not connect Disconnected signal";
		delete connection;
		QDBusConnection::disconnectFromBus ("ibus");
		return false;
	}

	ibus = connection;

	for (int i = 0; i < context_list.size (); i++ ) {
		QString ic = createInputContextRemote ();
		context_list[i]->setIC (ic);
		context_dict[ic] = context_list[i];
	}

	return true;
}

void
IBusClient::disconnectFromBus ()
{
	if (ibus) {
		delete ibus;
		ibus = NULL;
		QDBusConnection::disconnectFromBus ("ibus");
		for (int i = 0; i < context_list.size (); i++) {
			context_list[i]->setIC ("");
		}
		context_dict.clear ();
	}
}

void
IBusClient::slotDirectoryChanged (const QString & /*path*/)
{
	if (ibus && !ibus->isConnected ())
		disconnectFromBus ();

	if (ibus == NULL ) {
		QString session = getenv ("DISPLAY");
		session.replace (":", "-");
		QString path = QString("/tmp/ibus-%1/ibus-%2").arg (getenv ("USER"), session);
		if (QFile::exists (path)) {
			usleep (500);
			connectToBus ();
		}
	}
}

void
IBusClient::slotIBusDisconnected ()
{
	disconnectFromBus ();
}
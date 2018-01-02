#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>
#include <stdint.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>

static uint16_t SERVER_PORT = ACE_DEFAULT_SERVER_PORT;
static const char* const SERVER_HOST = ACE_DEFAULT_SERVER_HOST;
static const int MAX_ITERATIONS = 4;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("AuthExerciser");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription("SEGS AuthExerciser");

    qInfo().noquote().nospace() << parser.applicationDescription() << " v" << QCoreApplication::applicationVersion() << endl;

    parser.addPositionalArgument("serverHost", "The IP address authexerciser should connect to");
    parser.addPositionalArgument("serverPort", "The port authserver is listening to");
    parser.addPositionalArgument("maxIterations", "The maximum of messages to send authserver");

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    const char* arg0 = args[0].toLatin1().data();
    const char* serverHost = args.length() >= 1 ? arg0 : SERVER_HOST; // If args.length() > 1, serverHost = arg0. Else, SERVER_HOST.
    uint16_t serverPort = args.length() >= 2 ? ACE_OS::atoi(qPrintable(args[1])) : SERVER_PORT; // If args.length() >= 2, serverPort = qPrintable(args[1]). Else, SERVER_PORT.
    int maxIterations = args.length() >= 3 ? ACE_OS::atoi(qPrintable(args[2])) : MAX_ITERATIONS; // Same format as above.

    ACE_SOCK_Stream server;
    ACE_SOCK_Connector connector;
    ACE_INET_Addr addr(serverPort, serverHost);

    qInfo().noquote().nospace() << "IP and Port entered: " << arg0 << ":" << serverPort;
    qInfo().noquote().nospace() << "Attempting to connect to authserver...";

    if(connector.connect(server, addr) == -1)
        ACE_ERROR_RETURN((LM_ERROR, "%p\n", "open"), -1);
    else
        qInfo().noquote().nospace() << "Success!";

    for(int i = 0; i < maxIterations; i++)
    {
        char buf[BUFSIZ];

        ACE_OS::sprintf(buf, "message = %d\n", i + 1);

        if(server.send_n(buf, ACE_OS::strlen(buf)) == -1)
            ACE_ERROR_RETURN((LM_ERROR, "%p\n", "send"), -1);
        else
            ACE_OS::sleep(1);
    }

    if(server.close() == -1)
        ACE_ERROR_RETURN((LM_ERROR, "%p\n", "close"), -1);

    return 0;
}

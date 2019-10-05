#include "clientcontroller.h"

ClientController::ClientController()
{
    m_crdt = new CRDTclient(this);

    connect(&m_editor, &Editor::textChanged, this, [&](int position, int charsRemoved, int charsAdded) {
        if (charsAdded) {
            //qDebug() << "Added" << charsAdded << "chars at position" << position;
            QChar car =  m_editor.at(position);
            m_crdt->localInsert(position, car.toLatin1());
        } else {
            //qDebug() << "Removed" << charsRemoved << "chars at position" << position;
            //m_crdt.localDelete(position);
        }
    });

    connect(m_crdt, &CRDTclient::onLocalInsert, this, [&](){
        // TODO: create message
        qDebug() << "Creating message";
        m_socket.sendTextMessage("abbellodemamma");
    });

    m_socket.open(QUrl(QStringLiteral("ws://127.0.0.1:5678")));
    m_editor.show();
}

ClientController::~ClientController()
{
    delete m_crdt;
}

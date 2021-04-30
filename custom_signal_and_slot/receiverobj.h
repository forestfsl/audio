    #ifndef RECEIVEROBJ_H
    #define RECEIVEROBJ_H

    #include <QObject>

    class receiverObj : public QObject
    {
        Q_OBJECT
    public:
        explicit receiverObj(QObject *parent = nullptr);

    //自定义槽
    public slots:
        void handleExit();

    };

    #endif // RECEIVEROBJ_H

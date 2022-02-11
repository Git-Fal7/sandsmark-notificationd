#ifndef WIDGET_H
#define WIDGET_H

#include <QDialog>
#include <QTextBrowser>
#include <QLabel>

class QLabel;
class QTextBrowser;
class QTimer;

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    void setClickAction(const QString &action) { m_action = action; }
    void setNotificationId(const int id) { m_id = id; }

signals:
    void clicked(const int id, const QString &action);

protected:
    void mousePressEvent(QMouseEvent *event) override;
private:
    QString m_action;
    int m_id;
};

class Widget : public QDialog
{
    Q_OBJECT

public:
    Widget();
    ~Widget();

    void setNotificationId(const int id) {
        m_appIcon->setNotificationId(id); m_id = id;
        m_appName->setNotificationId(id); m_id = id;
    }
    void setDefaultAction(const QString &action);

    void setAppIcon(const QString &iconPath);
    void setAppIcon(const QImage &icon);

    void setAppText(const QString &name, const QString &summary, QString body);
    void setTimeout(int timeout);

public slots:
    void onCloseRequested(const int id);

signals:
    void actionClicked(const int id, const QString &action);
    void notificationClosed(const int id, const int reason);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void enterEvent(QEvent *event) override;

private slots:
    void onUrlClicked(const QUrl &url);

private:
    ClickableLabel *m_appIcon;
    ClickableLabel *m_appName;
    QTimer *m_dismissTimer;
    int m_id;
    int m_timeLeft = 0;
    int m_index = 0;

    static int s_visibleNotifications;
};
#endif // WIDGET_H

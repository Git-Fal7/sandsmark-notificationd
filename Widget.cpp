#include "Widget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QScreen>
#include <QDesktopServices>
#include <QIcon>

int Widget::s_visibleNotifications = 0;

Widget::Widget()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::WindowDoesNotAcceptFocus | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    if (s_visibleNotifications > 10) {
        qWarning() << "Too many visible notifications already";
        close();
    }
    m_index = ++s_visibleNotifications;

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(5);

    m_appIcon = new ClickableLabel;
    m_appIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mainLayout->addWidget(m_appIcon);

    m_appName = new ClickableLabel;
    mainLayout->addWidget(m_appName);

    connect(m_appIcon, &ClickableLabel::clicked, this, &Widget::actionClicked);
    connect(m_appName, &ClickableLabel::clicked, this, &Widget::actionClicked);

    m_dismissTimer = new QTimer(this);
    m_dismissTimer->setInterval(10000);
    m_dismissTimer->setSingleShot(true);
    connect(m_dismissTimer, &QTimer::timeout, this, &QWidget::close);
    m_dismissTimer->start();

    setVisible(true);
    adjustSize();
    clearFocus();
}

Widget::~Widget()
{
    s_visibleNotifications--;

    emit notificationClosed(m_id, 2); // always fake that the user clicked it away
}

void Widget::setAppText(const QString &name ,const QString &summary, QString body)
{
    m_appName->setText("<b>" + name + "</b><br/>" + summary + "<br/>" + body.replace("\n", "<br/>"));
}

void Widget::setDefaultAction(const QString &action)
{
    m_appIcon->setClickAction(action);
    m_appIcon->setCursor(Qt::PointingHandCursor);

    m_appName->setClickAction(action);
    m_appName->setCursor(Qt::PointingHandCursor);
}

void Widget::setAppIcon(const QString &iconPath)
{
    if (iconPath.isEmpty()) {
        qDebug() << "Tried to set empty icon";
        return;
    }

    QIcon icon = QIcon::fromTheme(iconPath);
    if (!icon.isNull()) {
        m_appIcon->setPixmap(icon.pixmap(32, 32));
        return;
    }

    QUrl iconUrl(iconPath);
    if (!iconUrl.isLocalFile()) {
        iconUrl = QUrl::fromLocalFile(iconPath);
    }
    setAppIcon(QImage(iconUrl.toLocalFile()));
}

void Widget::setAppIcon(const QImage &icon)
{
    if (icon.isNull()) {
        m_appIcon->setPixmap(QPixmap(":/annoying.png"));
        qWarning() << "Invalid icon";
        return;
    }

    m_appIcon->setPixmap(QPixmap::fromImage(
            icon.scaled(32, 32, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)
        ));
}

void Widget::setTimeout(int timeout)
{
    if (timeout <= 0) {
        timeout = 10;
    }

    m_dismissTimer->start(timeout * 1000);
}

void Widget::mousePressEvent(QMouseEvent *)
{
    close();
}

void Widget::resizeEvent(QResizeEvent *)
{
    const QRect screenGeometry = screen()->geometry();
    move((screenGeometry.width() / 2) - (width() / 2), 20);
}

void Widget::enterEvent(QEvent *)
{
    setWindowOpacity(1);
    m_timeLeft = m_dismissTimer->remainingTime();
    m_dismissTimer->stop();
}

void Widget::leaveEvent(QEvent *)
{
    setWindowOpacity(0.4); // slightly less visible on purpose, yes thank you
    m_dismissTimer->start(qMax(m_timeLeft, 0));
}

void Widget::onUrlClicked(const QUrl &url)
{
    qDebug() << "Clicked" << url;
}

void Widget::onCloseRequested(const int id)
{
    if (id == m_id) {
        close();
    }
}

void ClickableLabel::mousePressEvent(QMouseEvent *)
{
    if (m_action.isEmpty()) {
        return;
    }

    emit clicked(m_id, m_action);
}

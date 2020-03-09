#ifndef PRESS_F_FRAMEBUFFER_WIDGET_H
#define PRESS_F_FRAMEBUFFER_WIDGET_H

#include <QWidget>

#include "main.h"

class QPfFramebuffer : public QWidget
{
   Q_OBJECT

public:
   QPfFramebuffer(QWidget *parent);

   void setScale(const QSize& parent);

protected:
   QSize minimumSizeHint() const override;
   void  paintEvent(QPaintEvent *event) override;

private:
   QRect  m_Rect;
   u8     m_Scale;
   QImage m_Texture;
};

#endif

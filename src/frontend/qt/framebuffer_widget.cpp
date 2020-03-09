#ifndef PRESS_F_FRAMEBUFFER_WIDGET_CPP
#define PRESS_F_FRAMEBUFFER_WIDGET_CPP

#include <QPainter>
#include <QPaintEvent>

#include "framebuffer_widget.h"

extern "C"
{
   #include "../../screen.h"
}

QPfFramebuffer::QPfFramebuffer(QWidget *parent) : QWidget(parent)
{
   m_Rect = QRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
   m_Texture = QImage(SCREEN_WIDTH, SCREEN_HEIGHT, QImage::Format_RGB16);
}

QSize QPfFramebuffer::minimumSizeHint() const
{
   return QSize(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void QPfFramebuffer::paintEvent(QPaintEvent *event)
{
   QPainter painter(this);

   draw_frame_rgb565(g_ChannelF.vram, (u16*)m_Texture.bits());
   painter.drawImage(m_Rect, m_Texture);
}

void QPfFramebuffer::setScale(uint8_t scale)
{
   m_Scale = scale;
   m_Rect.setSize(QSize(SCREEN_WIDTH * scale, SCREEN_HEIGHT * scale));
}

#endif

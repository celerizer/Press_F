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

QSize QPfFramebuffer::getSize(void)
{
   return m_Rect.size();
}

QSize QPfFramebuffer::minimumSizeHint(void) const
{
   return QSize(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void QPfFramebuffer::paintEvent(QPaintEvent *event)
{
   QPainter painter(this);

   draw_frame_rgb565(g_ChannelF.vram, (u16*)m_Texture.bits());
   painter.drawImage(m_Rect, m_Texture);
}

bool QPfFramebuffer::setScale(const QSize& parent)
{
   u8 x = parent.width()  / SCREEN_WIDTH;
   u8 y = parent.height() / SCREEN_HEIGHT;
   u8 final_scale = x > y ? y : x;

   if (m_Scale != final_scale)
   {
      m_Scale = final_scale;
      m_Rect.setSize(QSize(SCREEN_WIDTH * final_scale, SCREEN_HEIGHT * final_scale));
      force_draw_frame();
      update();

      return true;
   }

   return false;
}

#endif

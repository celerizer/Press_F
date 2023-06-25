#ifndef PRESS_F_FRAMEBUFFER_WIDGET_CPP
#define PRESS_F_FRAMEBUFFER_WIDGET_CPP

#include <QPainter>
#include <QPaintEvent>

#include "framebuffer_widget.h"

extern "C"
{
  #include "../../screen.h"
  #include "../../hw/vram.h"
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

  Q_UNUSED(event)
  draw_frame_rgb565(((vram_t*)g_ChannelF.f8devices[3].device)->data, reinterpret_cast<u16*>(m_Texture.bits()));
  painter.drawImage(m_Rect, m_Texture);
}

bool QPfFramebuffer::setScale(const QSize& parent)
{
  auto x = static_cast<u8>(parent.width()  / SCREEN_WIDTH);
  auto y = static_cast<u8>(parent.height() / SCREEN_HEIGHT);
  auto final_scale = x > y ? y : x;

  /* Resize rect if needed */
  if (m_Scale != final_scale)
  {
    m_Scale = final_scale;
    m_Rect.setSize(QSize(
      SCREEN_WIDTH * final_scale, SCREEN_HEIGHT * final_scale));
  }

  /* Center horizontally */
  m_Rect.moveLeft((parent.width() - m_Rect.width()) / 2);

  force_draw_frame();
  update();

  return true;
}

#endif

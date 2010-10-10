/* This file is part of Clementine.

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tagwidget.h"
#include "ui/flowlayout.h"

#include <QPainter>
#include <QPropertyAnimation>

const int TagWidgetTag::kIconSize = 16;
const int TagWidgetTag::kIconTextSpacing = 6;
const int TagWidgetTag::kPadding = 2;

TagWidgetTag::TagWidgetTag(const QIcon& icon, const QString& text, QWidget* parent)
  : QWidget(parent),
    text_(text),
    icon_(icon),
    opacity_(0.0),
    animation_(new QPropertyAnimation(this, "background_opacity", this))
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QSize TagWidgetTag::sizeHint() const {
  QSize text = fontMetrics().size(0, text_);
  QSize icon(kIconSize + kIconTextSpacing, kIconSize);

  return QSize(kPadding + icon.width() + text.width() + kPadding,
               kPadding + qMax(text.height(), icon.height()) + kPadding);
}

void TagWidgetTag::set_background_opacity(float opacity) {
  opacity_ = opacity;
  update();
}

void TagWidgetTag::enterEvent(QEvent*) {
  animation_->stop();
  animation_->setEndValue(1.0);
  animation_->setDuration(80);
  animation_->start();
}

void TagWidgetTag::leaveEvent(QEvent*) {
  animation_->stop();
  animation_->setEndValue(0.0);
  animation_->setDuration(160);
  animation_->start();
}

void TagWidgetTag::paintEvent(QPaintEvent*) {
  QPainter p(this);

  const QRect tag_rect(rect());
  const QRect icon_rect(tag_rect.topLeft() + QPoint(kPadding, kPadding),
                        QSize(kIconSize, kIconSize));
  const QRect text_rect(icon_rect.topRight() + QPoint(kIconTextSpacing, 0),
                        QSize(tag_rect.width() - icon_rect.right() - kIconTextSpacing - kPadding,
                              icon_rect.height()));

  // Use the tag's opacity
  p.setOpacity(0.3 + opacity_ * 0.7);

  // Background
  QColor background_color = palette().color(QPalette::Highlight);
  background_color.setAlpha(128);

  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(QPen(palette().dark(), 1.0));
  p.setBrush(background_color);
  p.drawRoundedRect(tag_rect, 5, 5);

  // Icon
  p.drawPixmap(icon_rect, icon_.pixmap(kIconSize));

  // Text
  p.setOpacity(1.0);
  p.setPen(palette().color(QPalette::Text));
  p.drawText(text_rect, text_);
}


TagWidget::TagWidget(QWidget* parent)
  : QWidget(parent)
{
  setLayout(new FlowLayout);
}

void TagWidget::AddTag(const QString& tag) {
  if (tag.isEmpty())
    return;

  layout()->addWidget(new TagWidgetTag(icon_, tag, this));
}

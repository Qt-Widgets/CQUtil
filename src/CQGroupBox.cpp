#include <CQGroupBox.h>
#include <QChildEvent>
#include <QLayout>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOption>
#include <QMouseEvent>
#include <cmath>

CQGroupBox::
CQGroupBox(QWidget *parent) :
 QWidget(parent), title_()
{
  init();
}

CQGroupBox::
CQGroupBox(const QString &title, QWidget *parent) :
 QWidget(parent), title_(title)
{
  init();
}

CQGroupBox::
~CQGroupBox()
{
}

void
CQGroupBox::
init()
{
  setObjectName("groupBox");

  setTitleFont();

  lineColor_ = palette().color(QPalette::Mid);

  //----

  //area_ = new CQGroupBoxArea;

  //area_->setParent(this);

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void
CQGroupBox::
setTitleFont()
{
  titleFont_ = font();

  titleFont_.setPointSizeF(titleFont_.pointSizeF()*titleScale_);

  titleFont_.setBold(titleBold_);
}

void
CQGroupBox::
setTitle(const QString &title)
{
  title_ = title;

  update();
}

void
CQGroupBox::
setTitleBold(bool bold)
{
  titleBold_ = bold;

  setTitleFont();

  update();
}

void
CQGroupBox::
setTitleScale(double scale)
{
  titleScale_ = scale;

  setTitleFont();

  update();
}

void
CQGroupBox::
setTitleAlignment(Qt::Alignment alignment)
{
  titleAlignment_ = alignment;

  update();
}

void
CQGroupBox::
setHasLineTop(bool line)
{
  lineTop_ = line;

  update();
}

void
CQGroupBox::
setHasLineBottom(bool line)
{
  lineBottom_ = line;

  update();
}

void
CQGroupBox::
setLineTopAlignment(Qt::Alignment alignment)
{
  lineTopAlignment_ = alignment;

  update();
}

void
CQGroupBox::
setLineBottomAlignment(Qt::Alignment alignment)
{
  lineBottomAlignment_ = alignment;

  update();
}

void
CQGroupBox::
setMarginLeft(int margin)
{
  marginLeft_ = margin;

  calculateFrame();

  update();
}

void
CQGroupBox::
setMarginRight(int margin)
{
  marginRight_ = margin;

  calculateFrame();

  update();
}

void
CQGroupBox::
setMarginBottom(int margin)
{
  marginBottom_ = margin;

  calculateFrame();

  update();
}

void
CQGroupBox::
setMarginTop(int margin)
{
  marginTop_ = margin;

  calculateFrame();

  update();
}

void
CQGroupBox::
setCheckable(bool checkable)
{
  if (checkable != checkable_) {
    checkable_ = checkable;

    updateEnabled();

    update();
  }
}

void
CQGroupBox::
setChecked(bool checked)
{
  if (checked != checked_) {
    checked_ = checked;

    if (isCheckable()) {
      updateEnabled();

      emit toggled(isChecked());

      update();
    }
  }
}

void
CQGroupBox::
setCollapsible(bool collapsible)
{
  if (collapsible != collapsible_) {
    collapsible_ = collapsible;

    updateCollapsed();

    update();
  }
}

void
CQGroupBox::
setCollapsed(bool collapsed)
{
  if (collapsed != collapsed_) {
    collapsed_ = collapsed;

    if (isCollapsible()) {
      updateCollapsed();

      emit collapse(isCollapsed());

      update();
    }
  }
}

void
CQGroupBox::
changeEvent(QEvent *e)
{
  if (e->type() == QEvent::EnabledChange)
    updateEnabled();

  calculateFrame();

  QWidget::changeEvent(e);
}

void
CQGroupBox::
childEvent(QChildEvent *e)
{
  if (e->type() != QEvent::ChildAdded || ! e->child()->isWidgetType())
    return;

  calculateFrame();

  if (isCheckable())
    updateEnabled();

  QWidget::childEvent(e);
}

bool
CQGroupBox::
event(QEvent *e)
{
  if      (e->type() == QEvent::KeyPress) {
    QKeyEvent *ke = static_cast<QKeyEvent*>(e);

    if (ke->key() == Qt::Key_Select || ke->key() == Qt::Key_Space) {
      if (isCheckable()) {
        checkPress_ = true;

        update();

        return true;
      }
    }
  }
  else if (e->type() == QEvent::KeyRelease) {
    QKeyEvent *ke = static_cast<QKeyEvent*>(e);

    if (ke->key() == Qt::Key_Select || ke->key() == Qt::Key_Space) {
      if (isCheckable()) {
        setChecked(! isChecked());

        checkPress_ = false;

        emit clicked(isChecked());

        update();

        return true;
      }
    }
  }

  return QWidget::event(e);
}

void
CQGroupBox::
focusInEvent(QFocusEvent *e)
{
  QWidget::focusInEvent(e);
}

void
CQGroupBox::
mouseMoveEvent(QMouseEvent *e)
{
  if (isCheckable()) {
    bool inside = checkRect_.contains(e->pos()) || titleRect_.contains(e->pos());

    bool oldCheckPress = checkPress_;

    if      (! checkPress_ && inside)
      checkPress_ = true;
    else if (  checkPress_ && ! inside)
      checkPress_ = false;

    if (checkPress_ != oldCheckPress)
      update();
  }

  if (isCollapsible()) {
    bool inside = collapseRect_.contains(e->pos());

    bool oldCollapsePress = collapsePress_;

    if      (! collapsePress_ && inside)
      collapsePress_ = true;
    else if (  collapsePress_ && ! inside)
      collapsePress_ = false;

    if (collapsePress_ != oldCollapsePress)
      update();
  }

  QWidget::mouseMoveEvent(e);
}

void
CQGroupBox::
mousePressEvent(QMouseEvent *e)
{
  if (isCheckable()) {
    bool inside = checkRect_.contains(e->pos()) || titleRect_.contains(e->pos());

    if (inside) {
      checkPress_ = true;

      update();

      return;
    }
  }

  if (isCollapsible()) {
    bool inside = collapseRect_.contains(e->pos());

    if (inside) {
      collapsePress_ = true;

      update();

      return;
    }
  }

  QWidget::mousePressEvent(e);
}

void
CQGroupBox::
mouseReleaseEvent(QMouseEvent *e)
{
  if (isCheckable()) {
    bool inside = checkRect_.contains(e->pos()) || titleRect_.contains(e->pos());

    if (inside) {
      setChecked(! isChecked());

      checkPress_ = false;

      emit clicked(isChecked());

      update();
    }
  }

  if (isCollapsible()) {
    bool inside = collapseRect_.contains(e->pos());

    if (inside) {
      setCollapsed(! isCollapsed());

      collapsePress_ = false;

      update();
    }
  }

  QWidget::mouseReleaseEvent(e);
}

void
CQGroupBox::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.setRenderHints(QPainter::Antialiasing);

  p.fillRect(rect(), QBrush(palette().color(QPalette::Background)));

  QFontMetrics fm(titleFont_);

  // set check size
  int checkSize = (isCheckable() ? fm.height() + 4 : 0);

  // set collapse size
  int collapseSize = (isCollapsible() ? fm.ascent() : 0);

  // set text position
  int textX = 0;

  int tw = (title_ != "" ? fm.width(title_) : 0);

  if      (titleAlignment_ & Qt::AlignRight)
    textX = width() - dx_ - tw - collapseSize;
  else if (titleAlignment_ & Qt::AlignHCenter)
    textX = (width() - tw - checkSize - collapseSize)/2 + checkSize;
  else if (titleAlignment_ & Qt::AlignLeft)
    textX = dx_ + checkSize;

  int textY = 0;

  if      (titleAlignment_ & Qt::AlignBottom)
    textY = spaceTop() - fm.descent() - 2;
  else if (titleAlignment_ & Qt::AlignTop)
    textY = fm.ascent() + 2;
  else if (titleAlignment_ & Qt::AlignVCenter)
    textY = spaceTop()/2 + fm.descent() + 2;

  // set check position
  int checkX = textX - checkSize;
  int checkY = textY - fm.ascent()/2;

  // set collapse position
  int collapseX = width() - collapseSize;
  int collapseY = textY - fm.ascent()/2;

  // draw top line
  if (hasLineTop()) {
    int lineY;

    if      (lineTopAlignment_ & Qt::AlignTop)
      lineY = 2;
    else if (lineTopAlignment_ & Qt::AlignVCenter)
      lineY = spaceTop()/2;
    else
      lineY = spaceTop() - 2;

    p.setPen(lineColor_);

    p.drawLine(dx_, lineY, width() - 2*dx_, lineY);
  }

  // draw title
  if (title_ != "") {
    p.setFont(titleFont_);

    int tw1 = width() - checkSize - collapseSize - 4*dx_;

    int tw2 = std::min(tw, tw1);

    titleRect_ = QRect(textX - dx_, textY - fm.ascent() + fm.descent(), tw2 + dx_, fm.height());

    p.fillRect(titleRect_, QBrush(palette().color(QPalette::Background)));

    if (isEnabled())
      p.setPen(palette().color(QPalette::Active, QPalette::WindowText));
    else
      p.setPen(palette().color(QPalette::Disabled, QPalette::WindowText));

    p.drawText(textX, textY, title_);
  }

  // draw bottom line
  if (lineBottom_) {
    int lineY;

    if      (lineTopAlignment_ & Qt::AlignTop)
      lineY = height() - spaceBottom() + 2;
    else if (lineTopAlignment_ & Qt::AlignVCenter)
      lineY = height() - spaceBottom()/2;
    else
      lineY = height() - 2;

    p.setPen(lineColor_);

    p.drawLine(dx_, lineY, width() - 2*dx_, lineY);
  }

  //------

  // draw check (if checkable)
  if (isCheckable()) {
    int checkX1    = checkX + 2;
    int checkSize1 = checkSize - 4;
    int checkY1    = checkY - checkSize1/2;

    checkRect_ = QRect(checkX1, checkY1, checkSize1, checkSize1);

    p.fillRect(QRect(checkX1 - 2, checkY1 - 2, checkSize1 + 4, checkSize1 + 4),
               QBrush(palette().color(QPalette::Background)));

    QStylePainter p(this);

    QStyleOptionButton opt;

    opt.initFrom(this);

    opt.rect = checkRect_;

    opt.state |= (isChecked() ? QStyle::State_On : QStyle::State_Off);

    if (checkPress_)
      opt.state |= QStyle::State_Sunken;

#if 0
    if (testAttribute(Qt::WA_Hover) && underMouse()) {
      if (d->hovering)
        opt.state |= QStyle::State_MouseOver;
      else
        opt.state &= ~QStyle::State_MouseOver;
    }

    opt.text = d->text;
    opt.icon = d->icon;
    opt.iconSize = iconSize();
#endif

    p.drawControl(QStyle::CE_CheckBox, opt);
  }

  //------

  // draw collpase state (if collapsible)
  if (isCollapsible()) {
    double collapseSize1 = collapseSize - 4;

    double collapseX1 = collapseX + 2;
    double collapseY1 = collapseY - collapseSize1/2;
    double collapseX2 = collapseX1 + collapseSize1;
    double collapseY2 = collapseY1 + collapseSize1;

    collapseRect_ = QRect(collapseX1, collapseY1, collapseSize1, collapseSize1);

    p.fillRect(QRect(collapseX1 - 2, collapseY1 - 2, collapseSize1 + 4, collapseSize1 + 4),
               QBrush(palette().color(QPalette::Background)));

    double collapseXM = (collapseX1 + collapseX2)/2;
    double collapseYM = (collapseY1 + collapseY2)/2;

    p.setPen(Qt::NoPen);

    if (collapsePress_)
      p.setBrush(palette().color(QPalette::Active, QPalette::Dark));
    else
      p.setBrush(palette().color(QPalette::Active, QPalette::Text));

    if (! isCollapsed()) {
      drawArcShape(&p, collapseXM, collapseYM, collapseSize1/2, 90, 3);
    }
    else {
      drawArcShape(&p, collapseXM, collapseYM, collapseSize1/2, -90, 3);
    }
  }
}

void
CQGroupBox::
drawArcShape(QPainter *painter, double xc, double yc, double r, double startAngle, int sides) const
{
  auto Deg2Rad = [](double d) -> double { return M_PI*d/180.0; };
//auto Rad2Deg = [](double r) -> double { return 180.0*r/M_PI; };

  double x1 = xc - r;
  double y1 = yc - r;
  double x2 = xc + r;
  double y2 = yc + r;

  double xm = (x1 + x2)/2;
  double ym = (y1 + y2)/2;

  double da = 360.0/sides;
  double dc = 360.0/40;

  QPainterPath path;

  for (int i = 0; i < sides; ++i) {
    double angle = startAngle + i*da;

    double a1 = Deg2Rad(angle - dc);
    double a2 = Deg2Rad(angle + dc);

    double c1 = cos(a1), s1 = sin(a1);
    double c2 = cos(a2), s2 = sin(a2);

    QPointF p1(xm + r*c1, ym + r*s1);
    QPointF p2(xm + r*c2, ym + r*s2);

    if (i == 0)
      path.moveTo(p1);
    else
      path.lineTo(p1);

    //---

    QPointF p12 = (p1 + p2)/2;

    double ar = 2*hypot(p1.x() - p12.x(), p1.y() - p12.y())/sides;

    double a = Deg2Rad(angle);

    double c = cos(a), s = sin(a);

    QPointF pq(xm + (r + ar)*c, ym + (r + ar)*s);

    path.quadTo(pq, p2);
  }

  path.closeSubpath();

  painter->drawPath(path);
}

void
CQGroupBox::
resizeEvent(QResizeEvent *)
{
}

int
CQGroupBox::
spaceTop() const
{
  if (marginTop_ >= 0)
    return marginTop_;

  if (title_ != "") {
    QFontMetrics fm(titleFont_);

    return fm.height() + 4;
  }
  else
    return 4;
}

int
CQGroupBox::
spaceBottom() const
{
  if (marginBottom_ >= 0)
    return marginBottom_;

  return 4;
}

QSize
CQGroupBox::
sizeHint() const
{
  return minimumSizeHint();
}

QSize
CQGroupBox::
minimumSizeHint() const
{
  QFontMetrics fm(titleFont_);

  int baseWidth  = fm.width(title_) + 4;
  int baseHeight = fm.height();

  if (isCheckable())
    baseWidth += fm.height() + 4;

  if (isCollapsible())
    baseWidth += fm.height();

  QSize size;

  if (! isCollapsed()) {
    QStyleOptionGroupBox option;

    size = style()->sizeFromContents(QStyle::CT_GroupBox, &option,
                                     QSize(baseWidth, baseHeight), this);
  }
  else {
    size = QSize(baseWidth, baseHeight);
  }

  return size.expandedTo(QWidget::minimumSizeHint());
}

void
CQGroupBox::
calculateFrame()
{
  QLayout *l = layout();
  if (! l) return;

  l->setContentsMargins(marginLeft(), spaceTop(), marginRight(), spaceBottom());
}

void
CQGroupBox::
updateEnabled()
{
  bool enabled = isEnabled();

  QObjectList childList = children();

  for (int i = 0; i < childList.size(); ++i) {
    QObject *o = childList.at(i);

    if (! o->isWidgetType()) continue;

    QWidget *w = static_cast<QWidget *>(o);

    if (isCheckable()) {
      if (isChecked()) {
//      if (! w->isEnabled()) {
//        if (! w->testAttribute(Qt::WA_ForceDisabled))
            w->setEnabled(true);
//      }
      }
      else {
//      if (w->isEnabled())
          w->setEnabled(false);
      }
    }
    else {
      w->setEnabled(enabled);
    }
  }
}

void
CQGroupBox::
updateCollapsed()
{
  //area_->setVisible(isCollapsed());

  QObjectList childList = children();

  for (int i = 0; i < childList.size(); ++i) {
    QObject *o = childList.at(i);

    if (! o->isWidgetType()) continue;

    QWidget *w = static_cast<QWidget *>(o);

    w->setVisible(! isCollapsed());
  }

  QSize size = this->size();

  size.setHeight(minimumSizeHint().height());

  resize(size);
}

//-----------

#if 0
CQGroupBoxArea::
CQGroupBoxArea(QWidget *parent) :
 QWidget(parent)
{
}

void
CQGroupBoxArea::
paintEvent(QPaintEvent *)
{
  //QPainter p(this);

  //p.fillRect(rect(), QBrush(QColor(255,0,0)));
}
#endif

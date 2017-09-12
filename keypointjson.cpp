#include "keypointjson.h"

#include <QJsonObject>
#include <QJsonArray>

KeyPointJson::KeyPointJson(const QByteArray &data)
{
    QJsonParseError error;
    jDoc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        // error
        qt_assert("fail", __FILE__, __LINE__);
    }

    human();
}

QString KeyPointJson::id() {
    QJsonObject obj = jDoc.object();
    return obj["image_id"].toString();
}

QString KeyPointJson::url() {
    QJsonObject obj = jDoc.object();
    return obj["url"].toString();
}


static const char visible[NUM_VISUAL_STATUS] =
{
    '\0',
    'v', // visible,
    'i', // invisible
    'u'  // unknown
};

static QString humanToString(const HUMAN& h)
{
    QString str = h.name + ":\n";

    str += "body ";
    str += " [" + QString::number(h.rec.topLeft().x()) + ", " + QString::number(h.rec.topLeft().y()) + "], ";
    str += "[" + QString::number(h.rec.bottomRight().x()) + ", " + QString::number(h.rec.bottomRight().y()) + "]\n";

    //str += "keypoints:\n";
    str += "head ";
    str += "[" + QString::number(h.key[HEAD].p.x()) + ", " + QString::number(h.key[HEAD].p.y()) + "] " + visible[h.key[HEAD].v] +"\n";
    str += "neck ";
    str += "[" + QString::number(h.key[NECK].p.x()) + ", " + QString::number(h.key[NECK].p.y()) + "] " + visible[h.key[NECK].v] +"\n";
    str += "shoulder ";
    str += "[" + QString::number(h.key[RIGHT_SHOULDER].p.x()) + ", " + QString::number(h.key[RIGHT_SHOULDER].p.y()) + "] " + visible[h.key[RIGHT_SHOULDER].v] + " ";
    str += "[" + QString::number(h.key[LEFT_SHOULDER].p.x()) + ", " + QString::number(h.key[LEFT_SHOULDER].p.y()) + "] " + visible[h.key[LEFT_SHOULDER].v] + "\n";
    str += "elbow ";
    str += "[" + QString::number(h.key[RIGHT_ELBOW].p.x()) + ", " + QString::number(h.key[RIGHT_ELBOW].p.y()) + "] " + visible[h.key[RIGHT_ELBOW].v] + " ";
    str += "[" + QString::number(h.key[LEFT_ELBOW].p.x()) + ", " + QString::number(h.key[LEFT_ELBOW].p.y()) + "] " + visible[h.key[LEFT_ELBOW].v] + "\n";
    str += "wrist ";
    str += "[" + QString::number(h.key[RIGHT_WRIST].p.x()) + ", " + QString::number(h.key[RIGHT_WRIST].p.y()) + "] " + visible[h.key[RIGHT_WRIST].v] + " ";
    str += "[" + QString::number(h.key[LEFT_WRIST].p.x()) + ", " + QString::number(h.key[LEFT_WRIST].p.y()) + "] " + visible[h.key[LEFT_WRIST].v] + "\n";
    str += "hip ";
    str += "[" + QString::number(h.key[RIGHT_HIP].p.x()) + ", " + QString::number(h.key[RIGHT_HIP].p.y()) + "] " + visible[h.key[RIGHT_HIP].v] + " ";
    str += "[" + QString::number(h.key[LEFT_HIP].p.x()) + ", " + QString::number(h.key[LEFT_HIP].p.y()) + "] " + visible[h.key[LEFT_HIP].v] + "\n";
    str += "knee ";
    str += "[" + QString::number(h.key[RIGHT_KNEE].p.x()) + ", " + QString::number(h.key[RIGHT_KNEE].p.y()) + "] " + visible[h.key[RIGHT_KNEE].v] + " ";
    str += "[" + QString::number(h.key[LEFT_KNEE].p.x()) + ", " + QString::number(h.key[LEFT_KNEE].p.y()) + "] " + visible[h.key[LEFT_KNEE].v] + "\n";
    str += "ankle ";
    str += "[" + QString::number(h.key[RIGHT_ANKLE].p.x()) + ", " + QString::number(h.key[RIGHT_ANKLE].p.y()) + "] " + visible[h.key[RIGHT_ANKLE].v] + " ";
    str += "[" + QString::number(h.key[LEFT_ANKLE].p.x()) + ", " + QString::number(h.key[LEFT_ANKLE].p.y()) + "] " + visible[h.key[LEFT_ANKLE].v] + "\n";

    return str;
}

QString KeyPointJson::formatedInfo()
{
    QString info;
    info = "url: " + url() + "\n";
    info += "id: " + id() + "\n";

    foreach(HUMAN h, lh) {
        info += humanToString(h);
    }

    return info;
}

void KeyPointJson::human()
{
    QJsonObject obj = jDoc.object();

    QJsonObject jHuman = obj["human_annotations"].toObject();
    QJsonObject jKeypoint = obj["keypoint_annotations"].toObject();
    if (!jHuman.empty() && !jKeypoint.empty()) {
        foreach(QString key, jHuman.keys()) {
            HUMAN h;
            h.name = key;
            h.error = false;
            QJsonArray aValue = jHuman[key].toArray();
            if (aValue.size() != 4) {
                //memset(&h.rec, 0, sizeof(HUMAN_RECTANGLE));
                h.error = true;
            } else {
                h.rec.setTopLeft(QPoint(aValue.at(0).toInt(), aValue.at(1).toInt()));
                //h.rec.top_left.x = aValue.at(0).toInt();
                //h.rec.top_left.y = aValue.at(1).toInt();
                h.rec.setBottomRight(QPoint(aValue.at(2).toInt(), aValue.at(3).toInt()));
                //h.rec.bottom_right.x = aValue.at(2).toInt();
                //h.rec.bottom_right.y = aValue.at(3).toInt();
            }
            aValue = jKeypoint[key].toArray();
            if (aValue.size() != NUM_KEY_POINTS * 3) {
                memset(h.key, 0, sizeof(KEY_POINT) * NUM_KEY_POINTS);
                h.error = true;
            } else {
                for (int i = 0; i < NUM_KEY_POINTS; ++i) {
                    h.key[i].p.setX(aValue.at(3 * i).toInt());
                    h.key[i].p.setY(aValue.at(3 * i + 1).toInt());
                    h.key[i].v = (VISUAL_STATUS)(aValue.at(3 * i + 2).toInt());
                }
            }
            lh.push_back(h);
            if (h.error) {
                break;
            }
        }
    }
}


static const int WIDTH = 5;
static const int RADIUS = WIDTH / 2;
static const Qt::GlobalColor KEY_COLOR[NUM_VISUAL_STATUS] =
{
    Qt::white,
    Qt::red,
    Qt::magenta,
    Qt::gray
};

static const Qt::GlobalColor LINE_COLOR[NUM_VISUAL_STATUS] =
{
    Qt::white,
    Qt::green,
    Qt::yellow,
    Qt::gray
};

static const Qt::PenStyle LINE_STYLE[NUM_VISUAL_STATUS] =
{
    Qt::DotLine,
    Qt::SolidLine,
    Qt::SolidLine,
    Qt::DotLine
};

static void drawPoint(QGraphicsScene * scene, const KEY_POINT * p)
{
    Qt::GlobalColor color = KEY_COLOR[p->v];
    scene->addEllipse(p->p.x() - RADIUS, p->p.y() - RADIUS,
                      WIDTH, WIDTH, QPen(QBrush(color), 1), QBrush(color));
}

static void drawLine(QGraphicsScene * scene, const KEY_POINT * p0, const KEY_POINT * p1)
{
    VISUAL_STATUS v = p0->v > p1->v ? p0->v : p1->v;
    if (v != UNKNOWN) {
        Qt::GlobalColor c = LINE_COLOR[v];
        Qt::PenStyle s = LINE_STYLE[v];
        scene->addLine(p0->p.x(), p0->p.y(), p1->p.x(), p1->p.y(),
                       QPen(QBrush(c), 1, s));
    }
}

static void drawVirtualLine(QGraphicsScene * scene, const KEY_POINT * p0, const KEY_POINT * p1)
{
    VISUAL_STATUS v = p0->v > p1->v ? p0->v : p1->v;
    if (v != UNKNOWN) {
        Qt::GlobalColor c = Qt::GlobalColor::darkBlue;
        Qt::PenStyle s = Qt::DashLine;
        scene->addLine(p0->p.x(), p0->p.y(), p1->p.x(), p1->p.y(),
                       QPen(QBrush(c), 1, s));
    }
}

static void drawHuman(QGraphicsScene * scene, const HUMAN& h)
{
    // draw human body rectangle
    scene->addRect(h.rec, QPen(QBrush(Qt::yellow),1));

    // draw head
    drawPoint(scene, &h.key[HEAD]);

    // draw neck
    drawPoint(scene, &h.key[NECK]);

    // draw left shoulder
    drawPoint(scene, &h.key[LEFT_SHOULDER]);

    // draw right shoulder
    drawPoint(scene, &h.key[RIGHT_SHOULDER]);

    // draw left elbow
    drawPoint(scene, &h.key[LEFT_ELBOW]);

    // draw right elbow
    drawPoint(scene, &h.key[RIGHT_ELBOW]);

    // draw left wrist
    drawPoint(scene, &h.key[LEFT_WRIST]);

    // draw right wrist
    drawPoint(scene, &h.key[RIGHT_WRIST]);

    // draw left hip
    drawPoint(scene, &h.key[LEFT_HIP]);

    // draw right hip
    drawPoint(scene, &h.key[RIGHT_HIP]);

    // draw left knee
    drawPoint(scene, &h.key[LEFT_KNEE]);

    // draw right knee
    drawPoint(scene, &h.key[RIGHT_KNEE]);

    // draw left ankle
    drawPoint(scene, &h.key[LEFT_ANKLE]);

    // draw right ankle
    drawPoint(scene, &h.key[RIGHT_ANKLE]);

    // draw head-neck
    drawLine(scene, &h.key[HEAD], &h.key[NECK]);

    // draw neck-left shoulder
    drawLine(scene, &h.key[LEFT_SHOULDER], &h.key[NECK]);

    // draw neck-right shoulder
    drawLine(scene, &h.key[RIGHT_SHOULDER], &h.key[NECK]);

    // draw right shoulder-elbow
    drawLine(scene, &h.key[RIGHT_SHOULDER], &h.key[RIGHT_ELBOW]);

    // draw right elbow-wrist
    drawLine(scene, &h.key[RIGHT_WRIST], &h.key[RIGHT_ELBOW]);

    // draw left shoulder-elbow
    drawLine(scene, &h.key[LEFT_SHOULDER], &h.key[LEFT_ELBOW]);

    // draw left elbow-wrist
    drawLine(scene, &h.key[LEFT_WRIST], &h.key[LEFT_ELBOW]);

    // draw left hip-knee
    drawLine(scene, &h.key[LEFT_HIP], &h.key[LEFT_KNEE]);

    // draw left knee-ankle
    drawLine(scene, &h.key[LEFT_ANKLE], &h.key[LEFT_KNEE]);

    // draw right hip-knee
    drawLine(scene, &h.key[RIGHT_HIP], &h.key[RIGHT_KNEE]);

    // draw right knee-ankle
    drawLine(scene, &h.key[RIGHT_ANKLE], &h.key[RIGHT_KNEE]);

    // draw virtual right shoulder-hip
    drawVirtualLine(scene, &h.key[RIGHT_SHOULDER], &h.key[RIGHT_HIP]);

    // draw virtual left shoulder-hip
    drawVirtualLine(scene, &h.key[LEFT_SHOULDER], &h.key[LEFT_HIP]);

    // draw virtual hip
    drawVirtualLine(scene, &h.key[LEFT_HIP], &h.key[RIGHT_HIP]);
}

void KeyPointJson::drawHumans(QGraphicsScene * scene)
{
    foreach(HUMAN h, lh) {
        drawHuman(scene, h);
    }
}

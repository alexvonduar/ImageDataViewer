#ifndef KEYPOINTJSON_H
#define KEYPOINTJSON_H

#include <QJsonDocument>
#include <QList>
#include <QGraphicsScene>

typedef enum {
    VISIBLE = 1,
    INVISIBLE = 2,
    UNKNOWN = 3,
    NUM_VISUAL_STATUS
} VISUAL_STATUS;

typedef struct {
    QPoint p;
    VISUAL_STATUS v;
} KEY_POINT;

typedef enum {
    //1/右肩，2/右肘，3/右腕，4/左肩，5/左肘，6/左腕，7/右髋，8/右膝，9/右踝，10/左髋，11/左膝，12/左踝，13/头顶，14/脖子

    RIGHT_SHOULDER = 0,
    RIGHT_ELBOW,
    RIGHT_WRIST,
    LEFT_SHOULDER,
    LEFT_ELBOW,
    LEFT_WRIST,
    RIGHT_HIP,
    RIGHT_KNEE,
    RIGHT_ANKLE,
    LEFT_HIP,
    LEFT_KNEE,
    LEFT_ANKLE,
    HEAD,
    NECK,
    NUM_KEY_POINTS
} KEY_POINT_POSITION;

typedef struct
{
    QString name;
    QRect rec;
    KEY_POINT key[NUM_KEY_POINTS];
    bool error;
} HUMAN;

class KeyPointJson
{
public:
    KeyPointJson(const QByteArray& data);

    QString id();

    QString url();

    QString formatedInfo();

    QList<HUMAN> get() { return lh; }

    void drawHumans(QGraphicsScene *scene);

private:
    QJsonDocument jDoc;
    QList<HUMAN> lh;

    void human();
};

#endif // KEYPOINTJSON_H

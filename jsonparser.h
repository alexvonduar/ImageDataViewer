#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QList>
#include <QFile>

typedef struct {
    qint64 offset;
    qint64 len;
} Item;

class JsonParser
{
public:
    JsonParser();
    int size() { return items.size(); }
    Item get(int idx) {
        if (idx >= items.size()) {
            Item it = {0, 0};
            return it;
        }
        return items[idx];
    }
    QByteArray read(int idx) {
        if (idx >= items.size()) {
            return QByteArray();
        }
        file.seek(items[idx].offset);
        QByteArray ar = file.read(items[idx].len);
        if (ar.length() != items[idx].len) {
            return QByteArray();
        }
        return ar;
    }
    void Parse(const QString& filePath);

private:
    QList<Item> items;
    QFile file;
};

#endif // JSONPARSER_H

#include "jsonparser.h"

#include <QStack>

JsonParser::JsonParser()
{
}

void JsonParser::Parse(const QString &filePath)
{
    const static int BUFFER_SIZE = 1024 * 1024;

    file.close();

    file.setFileName(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    items.clear();

    file.seek(0);

    qint64 start = 0;
    int len = 0;
    int last_len = 0;
    QStack<char> st;
    qint64 off = 0;

    while (!file.atEnd()) {
        QByteArray buffer = file.read(BUFFER_SIZE);
        bool haveError = false;
        for (int i = 0; i < buffer.size(); ++i) {
            char c = buffer.at(i);
            if (st.size() && (st.top() == ']' || st.top() == '}')) {
                haveError = true;
                break;
            }
            switch(c) {
            case '[':
                if (last_len != 0 || st.size() != 0) {
                    // skip first [
                    st.push(c);
                }
                break;
            case '{':
                if (st.size() == 0) {
                    // item start
                    st.push(c);
                    start = off;// + 1;
                    last_len = len;
                    len = 0;
                } else {
                    st.push(c);
                }
                break;
            case ']':
                if (st.size() && st.top() == '[') {
                    st.pop();
                } else {
                    // last ];
                    st.push(c);
                }
                break;
            case '}':
                if (st.size() && st.top() == '{') {
                    st.pop();
                }
                if (st.size() == 0) {
                    // one item complete
                    len = off - start + 1;
                    Item it = {start, len};
                    items.push_back(it);
                }
                break;
             default:
                break;
            }
            off += 1;
        }

        if (haveError) {
            break;
        }
    }
}

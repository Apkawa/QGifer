#include "qgiferformatter.h"


namespace QGifer {
    namespace utils {

        QString humanSizeFormat(unsigned long byteSize) {
            float num = (float) byteSize;
            QStringList list;
            list << "KB" << "MB" << "GB" << "TB";

            QStringListIterator i(list);
            QString unit("bytes");

            while (num >= 1024.0 && i.hasNext()) {
                unit = i.next();
                num /= 1024.0;
            }
            return QString().setNum(num, 'f', 2) + " " + unit;
        }

    }
}

#ifndef _TARGET_CHOOSER_H
#define _TARGET_CHOOSER_H
#include "ArmorFinder.h"

namespace nw{
    class TargetChooser{
        public:
            TargetChooser();
            ~TargetChooser();
            ArmorBlobs getAimTarget(ArmorBlobs &blobsm, int last_id);
            int getTargetIndice() { return target_indice; }
            void setTopPri(int pri) { top_pri = pri;}
        private:
            int getPriority(int id, int last_id);

            static float getDistanceToCenter(const ArmorBlob &blob);
            static bool compFunc(const ArmorBlob &blob1, const ArmorBlob &blob2);
            void getTargetOrder(ArmorBlobs &blobs);
            int top_pri;        
            int target_indice;
            cv::Point2f last_armor_center;
    };
}
#endif // _TARGET_CHOOSER_H
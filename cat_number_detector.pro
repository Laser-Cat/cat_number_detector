TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    cat_classifier.cpp \
    cat_head.cpp \
    cat_general_classifier_api.cpp



HEADERS += \
    cat_classifier.h \
    cat_head.h \
    cat_general_classifier_api.h



QMAKE_CXXFLAGS += -std=c++11

LIBS+= -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_ocl -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab

DISTFILES += \
    heat_mask.png \
    back_ground_sample_mask_.bmp \
    readme.txt


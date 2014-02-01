# ScrabbleEye #

an automatic computer-vision powered referee for your game of SCRABBLE


# How to build #

Essentially go the standard CMake route. You will have to compile the OpenCV libraries beforehand, also using CMake. After performing the first "Configure" in cmake-gui, most likely the OpenCV libraries will not get found. You will need to find the OpenCV_DIR variable and set it to the path to the OpenCV **build** directory. Google Test and Google Mock should be downloaded and built automatically.

Make sure you build OpenCV correctly, especially the "WITH" section. We need WITH_GTK and without CUDA and OpenCL for less setup headache.


## Action plan ##

1. Hue / saturation filter to get the red pixels
2. k-means clustering to locate the triple word score fields
3. excluding the largest (count OR size OR even better the standard deviation from the center of the cluster) cluster to get orientation and exclude the SCRABBLE text
3. Locating corner pixels of the corner Triple Word fields 
4. Finding homography by field edges
5. modifying perspective matrix to compensate the thickness of the pieces
6. identifying if piecas are in spot by amount of yellow (brute pixel by pixel color difference from a whole yellow block?)?
7. piece recognition by SVM/kNN/NN
8. ???
9. PROFIT


## Locating Corner pixels of the Triple Word fields ##

1. Creating a snake-like list of clusters
2. The head and tail of the list are corners
3. The furthest pixels of the opposite corners are the corners of the playfield

## Random package names ##

build-essential libgtk2.0-dev libjpeg-dev libtiff4-dev libjasper-dev libopenexr-dev cmake python-dev python-numpy python-tk libtbb-dev libeigen2-dev yasm libfaac-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev libx264-dev libqt4-dev libqt4-opengl-dev sphinx-common texlive-latex-extra libv4l-dev libdc1394-22-dev libavcodec-dev libavformat-dev libswscale-dev 

libgstreamer0.10-0 libgstreamer0.10-dev gstreamer0.10-tools gstreamer0.10-plugins-base libgstreamer-plugins-base0.10-dev gstreamer0.10-plugins-good gstreamer0.10-plugins-ugly gstreamer0.10-plugins-bad gstreamer0.10-ffmpeg

libavcodec-dev libavformat-dev libswscale-dev

v4l2 libv4l-dev

libv4l-0 libv4l-dev

Something with ffmpeg?

sudo apt-get install build-essential libgtk2.0-dev libjpeg-dev libtiff4-dev libjasper-dev libopenexr-dev cmake python-dev python-numpy python-tk libtbb-dev libeigen2-dev yasm libfaac-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev libx264-dev libqt4-dev libqt4-opengl-dev sphinx-common texlive-latex-extra libv4l-dev libdc1394-22-dev libavcodec-dev libavformat-dev libswscale-dev

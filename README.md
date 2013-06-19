(this project is suspended. It might be rewritten to Scala and continued in summer 2013)

# ScrabbleEye #

an automatic computer-vision powered referee for your game of SCRABBLE

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

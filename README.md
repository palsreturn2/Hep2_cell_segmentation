# Hep2_cell_segmentation

# Synopsis

This is a image segmentation project that uses region growing technique for segmenting Hep-2 cell (For more info on Hep2 cell: http://www.birmingham.ac.uk/facilities/clinical-immunology-services/autoimmunity/hep-2-image-library/index.aspx). The region growing technique used is a cellular automaton algorithm known as interactive growcut. Interactive growcut is preceded by an automatic marker generation to get rid of the human intervention portion of the growcut algorithm(for more info on growcut: http://graphicon.ru/html/2005/proceedings/papers/VezhntvetsKonushin.pdf). The marker generation extensively uses morphological reconstruction, erosion-dilation and convex hull algorithm. The generated markers are then applied to growcut for final segmentation.

This project also consists of a sequential version of growcut developed by us that is faster than the original parallel algorithm on a sequential machine. It can be proved that the sequential version of the algorithm gives the same result as the original growcut using contradiction The function is named as "syncgrowCut" algorithm and is present in "growcut.c".

# Installation and running

Steps to execute the code.

1) Download a Hep2 cell image from google .(The images must be in .pgm format)

2) The image needs to be manually segmented inorder to check the segmentation accuracy.

3) Download the project in a folder and run "chmod 755 Run"

4) Execute "./Run"

5) Then execute "./segment <cellimage> <groundtruthimage>"


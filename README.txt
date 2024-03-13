3-12_WriteBinRapid.cpp: writes sample images to a binary .bin file. Currently writes a series of 5 images with slight differences for rapid reading by 3-12_BinToSDK.cpp


3-12_BinToSDK.cpp: Uses one thread to read a binary .bin file with image data from 3-12_WriteBinRapid.cpp and copy into an image array. Simultaneously uses another thread to read image array data in parallel - this thread will be replaced by a function to communicate image data to SLM

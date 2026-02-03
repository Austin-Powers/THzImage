# THzImage
Collection of classes, functions and templates for image processing.

## Contents
- __analysis__ Classes and functions for analyzing images
- __common__ Common image related classes used throughout the library
- __handling__ Classes related to storing multiple images in memory
- __io__ Classes realted to reading and writing image from and to files
- __processing__ Classes for conventiently building processing chains for images
- __transformation__ Classes for transforming images data

## Details
Below is more detailed information about the contents of each subdirectory.

### Analysis
- __`class BasicImageMetrics`__ _(basicImageMetrics.hpp)_ Calculates a basic set of metrics of the given image.
  

### Common
- __`class DisplayServerConnection`__ _(displayserver.hpp)_ A connection to the display server.
  
- __`class Image;`__ _(iImageReader.hpp)_ Forward declaration of Image for convenience function readInto.
- __`class IImageReader`__ _(iImageReader.hpp)_ Interface for all classes reading images.
- __`concept ImageReader`__ _(iImageReader.hpp)_ Concept of a ImageReader.
- __`concept FileImageReader`__ _(iImageReader.hpp)_ Concept of a FileImageReader.
  
- __`class IImageTransformer`__ _(iImageTransformer.hpp)_ Interface for all classes performing transformations on image data.
- __`concept ImageTransformer`__ _(iImageTransformer.hpp)_ Concept for ImageTransformers.
  
- __`class Image;`__ _(iImageWriter.hpp)_ Forward declaration of Image for convenience function writeContentOf.
- __`class IImageWriter`__ _(iImageWriter.hpp)_ Interface for all classes writing images.
- __`concept ImageWriter`__ _(iImageWriter.hpp)_ Concept of a ImageWriter.
- __`concept FileImageWriter`__ _(iImageWriter.hpp)_ Concept of a FileImageWriter.
  
- __`struct ImageProject`__ _(image.hpp)_ Name provider for the THzImage.Common.Image class.
- __`class Image`__ _(image.hpp)_ Class representing raster based images.
- __`definition BGRAImage`__ _(image.hpp)_ Using declaration for an image using BGRAPixel.
- __`definition HSVAImage`__ _(image.hpp)_ Using declaration for an image using HSVAPixel.
- __`definition MiniHSVImage`__ _(image.hpp)_ Using declaration for an image using MiniHSVPixel.
  
- __`class ImageView`__ _(imageView.hpp)_ A view into a certain region of an image or pixel buffer. Doubles as the starting point of a IImageTransformer chain.
- __`definition BGRAImageView`__ _(imageView.hpp)_ Using declaration for an image view using BGRAPixel.
- __`definition BGRAImageView`__ _(imageView.hpp)_ Using declaration for an image view using HSVAPixel.
  
- __`struct BGRAPixel`__ _(pixel.hpp)_ Struct for a blue green read alpha pixel using 8 bits per channel.
- __`struct TemplatedBGRAPixel`__ _(pixel.hpp)_ Struct for a blue green read alpha pixel using a custom data type for the channels. This struct offers operators for doing math with the regular BGRAPixel, to for instance calculate the average color of a set of pixels.
- __`struct HSVAPixel`__ _(pixel.hpp)_ Struct for HSVA pixel.
- __`struct MiniHSVPixel`__ _(pixel.hpp)_ Struct for a size reduced HSV pixel.
- __`definition BGRAPixelFloat`__ _(pixel.hpp)_ Shortcut to a templated BGRAPixel class using float.
- __`definition BGRAPixel32`__ _(pixel.hpp)_ Shortcut to a templated BGRAPixel class using std::uint32_t.
- __`concept Pixel`__ _(pixel.hpp)_ Concept for a pixel type.
  

### Handling
- __`class AsyncImageRingBuffer`__ _(asyncImageRingBuffer.hpp)_ Extends the basic ImageRingBuffer by adding the ability to retrieve the reader or transformer result asynchronously.
  
- __`class BufferTransformer`__ _(bufferTransformer.hpp)_ Transformer starting a chain of transformers from a imageRingBuffer.
  
- __`class ImageRingBuffer`__ _(imageRingBuffer.hpp)_ A Ringbuffer for image handling.
  

### Io
- __`class AsyncWriter`__ _(asyncWriter.hpp)_ Uses a given writer to write images asynchronously.
  
- __`class Reader`__ _(autoFileReader.hpp)_ File reader that automatically checks the file type and opens the correct one.
  
- __`class Reader`__ _(bmpReader.hpp)_ Reads an image from a file using the BitMap format.
  
- __`class Writer`__ _(bmpWriter.hpp)_ Writes an image to a file using the BitMap format.
  
- __`class Reader`__ _(gifReader.hpp)_ Reads an image from a file using the GIF format.
  
- __`class ColorReduction`__ _(gifWriter.hpp)_ Encapsulates the algorithm for reducing the colors of a given image to 255.
- __`class Dithering`__ _(gifWriter.hpp)_ Encapsulates the dithering algorithm.
- __`class Writer`__ _(gifWriter.hpp)_ Writes an image to a file using the GIF format.
  
- __`class Reader`__ _(imageDirectoryReader.hpp)_ Reads all images from a directory.
  
- __`struct WriterProject`__ _(imageSeriesWriter.hpp)_ Name provider for the THzImage.IO.ImageSeries.Writer class.
- __`class Writer`__ _(imageSeriesWriter.hpp)_ Wrapper for other writers, enabling writing of multiple images.
  
- __`class Reader`__ _(pngReader.hpp)_ Reads an image from a file using the Portable-Network-Graphics format.
  
- __`class Writer`__ _(pngWriter.hpp)_ Writes an image to a file using the Portable-Network-Graphics format.
  
- __`class Decompressor`__ _(qoiReader.hpp)_ Class containing the QOI decompression algorithm for testing purposes.
- __`class Reader`__ _(qoiReader.hpp)_ Reads an image from a file using the Quite-Okay-Image format.
  
- __`class Compressor`__ _(qoiWriter.hpp)_ Class containing the QOI compression algorithm for testing purposes.
- __`class Writer`__ _(qoiWriter.hpp)_ Writes an image to a file using the Quite-Okay-Image format.
  
- __`class Reader`__ _(screenReader.hpp)_ 
  
- __`class TestImageGenerator`__ _(testImageGenerator.hpp)_ Reader for generating a test image.
  
- __`class TimedReader`__ _(timedReader.hpp)_ Wraps another reader class to add the ability to let time pass between reading cycles.
  

### Processing
- __`class DataReductionNode`__ _(dataReductionNode.hpp)_ Reduces the data amount of processed images by downscaling and converting to MiniHSVPixel-format.
  
- __`class EasyWriter`__ _(easyWriter.hpp)_ Writes images directly to enumerated PNG-files for debugging or storage purposes.
  
- __`class FileInputNode`__ _(fileInputNode.hpp)_ Node for loading a directory of images, not including sub-directories.
  
- __`class ImageInputNode`__ _(imageInputNode.hpp)_ Node for putting a single image into the processing chain. This can be used to perform processing on a single image or to manipulate the image between processing runsssss.
  
- __`enum ToCountResult`__ _(iNode.hpp)_ The different results that can be produced by the toCount method of an node.
- __`class INode`__ _(iNode.hpp)_ The interface used by all image processing nodes.
  
- __`class ReaderlessNodeBase`__ _(readerlessNodeBase.hpp)_ A common base for all nodes that do not wrap a reader to work.
  
- __`class ScreenInputNode`__ _(screenInputNode.hpp)_ Node for taking screenshots as an image source.
  
- __`class TestInputNode`__ _(testInputNode.hpp)_ Node for inserting test images into the node system.
  

### Transformation
- __`struct Borders`__ _(borderTransformer.hpp)_ Contains the borders to add to an image.
- __`class BorderTransformer`__ _(borderTransformer.hpp)_ Transformer adding a border to an image.
  
- __`class LineBuffer`__ _(convolutionTransformer.hpp)_ Stores the lines needed for running the transformation.
- __`class MatrixHelper`__ _(convolutionTransformer.hpp)_ Used for stepping through the line buffer and creating the matrizes for the transformation.
- __`struct ConvolutionTransformerProject`__ _(convolutionTransformer.hpp)_ Name provider for the THzImage.IO.BMP.Reader class.
- __`class ConvolutionParameters`__ _(convolutionTransformer.hpp)_ Checks and stores the paramters of convolution transformation.
- __`concept ConvolutionTransformation`__ _(convolutionTransformer.hpp)_ 
- __`class ConvolutionTransformer`__ _(convolutionTransformer.hpp)_ Class wrapping Pixel-Matrix-to-Pixel transformation algorithms        to make them implement the IImageTransformer interface.
  
- __`struct ConvolutionTransformerProject`__ _(convolutionTransformerBase.hpp)_ Name provider for the THzImage.Transformation.Convolution project.
- __`class ConvolutionTransformerBase`__ _(convolutionTransformerBase.hpp)_ Base class for convolution based transformations.
  
- __`class NullTransformer`__ _(nullTransformer.hpp)_ Enables default construction of IImageTransformers without the need for code in these classes handling default construction.
  
- __`concept PixelTransformation`__ _(pixelTransformer.hpp)_ Concept of a class transforming pixels of TPixelType.
- __`class PixelTransformer`__ _(pixelTransformer.hpp)_ Class wrapping Pixel-to-Pixel transformation algorithms        to make them implement the IImageTransformer interface.
  


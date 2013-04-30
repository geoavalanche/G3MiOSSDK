//
//  BilParser.cpp
//  G3MiOSSDK
//
//  Created by Diego Gomez Deck on 2/19/13.
//
//

#include "BilParser.hpp"

#include "IByteBuffer.hpp"
#include "ByteBufferIterator.hpp"
//#include "IFloatBuffer.hpp"
#include "IShortBuffer.hpp"
#include "IFactory.hpp"
#include "ShortBufferElevationData.hpp"
//#include "FloatBufferElevationData.hpp"
#include "Vector2I.hpp"


ShortBufferElevationData* BilParser::parseBil16(const Sector& sector,
                                                const Vector2I& extent,
                                                short noDataValue,
                                                const IByteBuffer* buffer) {

  const int size = extent._x * extent._y;

  const int expectedSizeInBytes = size * 2;
  if (buffer->size() != expectedSizeInBytes) {
    ILogger::instance()->logError("Invalid buffer size, expected %d bytes, but got %d",
                                  expectedSizeInBytes,
                                  buffer->size());
    return NULL;
  }

  ByteBufferIterator iterator(buffer);

  const short minValue = IMathUtils::instance()->minInt16();

  IShortBuffer* shortBuffer = IFactory::instance()->createShortBuffer(size);
  for (int i = 0; i < size; i++) {
    short height = iterator.nextInt16();

    if (height == -9999) {
      height = noDataValue;
    }
    else if (height == minValue) {
      height = noDataValue;
    }

    shortBuffer->rawPut(i, height);
  }

  return new ShortBufferElevationData(sector,
                                      extent,
                                      noDataValue,
                                      shortBuffer);
}

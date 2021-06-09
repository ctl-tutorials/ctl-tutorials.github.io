#ifndef RAWDATAIO_H
#define RAWDATAIO_H

#include "io/metainfokeys.h"

#include <QDebug>
#include <QFile>
#include <QVariantMap>

template<uint dim1, uint dim2, uint dim3, typename RawType>
class RawDataIO
{
public:
    QVariantMap metaInfo(const QString& fileName) const;
    template <typename T>
    std::vector<T> readAll(const QString& fileName) const;
    template <typename T>
    std::vector<T> readChunk(const QString& fileName, uint chunkNb) const;
    template <typename T>
    bool write(const std::vector<T>& data, const QVariantMap& metaInfo, const QString& fileName) const;
};


template<uint dim1, uint dim2, uint dim3, typename RawType>
QVariantMap RawDataIO<dim1, dim2, dim3, RawType>::metaInfo(const QString&) const
{
    QVariantMap ret;

    CTL::io::meta_info::Dimensions dimensions( dim1, dim2, dim3 );

    ret.insert(CTL::io::meta_info::dimensions, QVariant::fromValue(dimensions));

    return ret;
}

template<uint dim1, uint dim2, uint dim3, typename RawType>
template<typename T>
std::vector<T> RawDataIO<dim1, dim2, dim3, RawType>::readAll(const QString& fileName) const
{
    QFile infile(fileName);
    if(!infile.open(QIODevice::ReadOnly))
    {
        qCritical() << "Could not open file " << fileName << "for reading.";
        return std::vector<T>();
    }

    std::vector<RawType> rawValues(dim1 * dim2 * dim3);
    const auto numBytes = rawValues.size() * sizeof(RawType);
    const auto bytesRead = infile.read(reinterpret_cast<char*>(rawValues.data()), numBytes);
    infile.close();

    if(numBytes != bytesRead)
        qWarning() << "Did not read the expected amount of data.";

    // conversion from RawType -> T
    std::vector<T> ret(rawValues.size());
    std::transform(rawValues.cbegin(), rawValues.cend(), ret.begin(),
                   [] (const RawType& value) { return static_cast<T>(value); } );

    return ret;
}

template<uint dim1, uint dim2, uint dim3, typename RawType>
template<typename T>
std::vector<T> RawDataIO<dim1, dim2, dim3, RawType>::readChunk(const QString &fileName, uint chunkNb) const
{
    QFile infile(fileName);
    if(!infile.open(QIODevice::ReadOnly))
    {
        qCritical() << "Could not open file " << fileName << "for reading.";
        return std::vector<T>();
    }

    std::vector<RawType> rawValues(dim1 * dim2);
    const auto numBytes = rawValues.size() * sizeof(RawType);
    infile.skip(dim1 * dim2 * sizeof(RawType) * chunkNb);
    const auto bytesRead = infile.read(reinterpret_cast<char*>(rawValues.data()), numBytes);
    infile.close();

    if(numBytes != bytesRead)
        qWarning() << "Did not read the expected amount of data.";

    // conversion from RawType -> T
    std::vector<T> ret(rawValues.size());
    std::transform(rawValues.cbegin(), rawValues.cend(), ret.begin(),
                   [] (const RawType& value) { return static_cast<T>(value); } );

    return ret;
}

template<uint dim1, uint dim2, uint dim3, typename RawType>
template<typename T>
bool RawDataIO<dim1, dim2, dim3, RawType>::write(const std::vector<T> &data, const QVariantMap&, const QString &fileName) const
{
    QFile outfile(fileName);
    if(!outfile.open(QIODevice::WriteOnly))
    {
        qCritical() << "Could not open file " << fileName << "for writing.";
        return false;
    }

    const auto numElements = static_cast<size_t>(dim1) * dim2 * dim3;

    // convert input data into correct data type (T -> RawType)
    std::vector<RawType> rawValues(numElements);
    std::transform(data.cbegin(), data.cend(), rawValues.begin(),
                   [] (const T& value) { return static_cast<RawType>(value); } );

    // write the data
    const auto numBytes = numElements * sizeof (RawType);
    const auto bytesWritten = outfile.write(reinterpret_cast<const char*>(rawValues.data()), numBytes);

    outfile.close();

    return bytesWritten == numBytes;
}



#endif // RAWDATAIO_H

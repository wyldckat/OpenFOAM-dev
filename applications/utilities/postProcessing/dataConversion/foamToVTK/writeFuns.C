/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2016 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "writeFuns.H"
#include "vtkTopo.H"

#if defined(__mips)
#include <standards.h>
#include <sys/endian.h>
#endif

// MacOSX
#ifdef __DARWIN_BYTE_ORDER
#if __DARWIN_BYTE_ORDER==__DARWIN_BIG_ENDIAN
#undef LITTLE_ENDIAN
#else
#undef BIG_ENDIAN
#endif
#endif

#if defined(LITTLE_ENDIAN) \
 || defined(_LITTLE_ENDIAN) \
 || defined(__LITTLE_ENDIAN)
#   define LITTLEENDIAN 1
#elif defined(BIG_ENDIAN) || defined(_BIG_ENDIAN) || defined(__BIG_ENDIAN)
#   undef LITTLEENDIAN
#else
#   error "Cannot find LITTLE_ENDIAN or BIG_ENDIAN symbol defined."
#   error "Please add to compilation options"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void Foam::writeFuns::swapWord32(char* word32)
{
    char a = word32[0];
    word32[0] = word32[3];
    word32[3] = a;

    a = word32[1];
    word32[1] = word32[2];
    word32[2] = a;
}


void Foam::writeFuns::swapWord64(char* word64)
{
    char a = word64[0];
    word64[0] = word64[7];
    word64[7] = a;

    a = word64[1];
    word64[1] = word64[6];
    word64[6] = a;

    a = word64[2];
    word64[2] = word64[5];
    word64[5] = a;

    a = word64[3];
    word64[3] = word64[4];
    word64[4] = a;
}


void Foam::writeFuns::swapWords
(
    const label nWords,
    char* words,
    const label wordSize
)
{
    if(wordSize==4)
    {
        char* word32 = words;
        for (label w = 0; w < nWords; word32+=wordSize, w++)
        {
            swapWord32(word32);
        }
    }
    else if(wordSize==8)
    {
        char* word64 = words;
        for (label w = 0; w < nWords; word64+=wordSize, w++)
        {
            swapWord64(word64);
        }
    }
    else
    {
        FatalErrorInFunction
            << "unsupported word size of " << wordSize << nl
            << "Please report this issue on the bug tracker and please provide"
            << " complete details for reproducing this error."
            << exit(FatalError);

    }
}


void Foam::writeFuns::write
(
    std::ostream& os,
    const bool binary,
    List<scalar>& fField
)
{
    if (binary)
    {
#       ifdef LITTLEENDIAN
        swapWords
        (
            fField.size(),
            reinterpret_cast<char*>(fField.begin()),
            sizeof(scalar)
        );
#       endif
        os.write
        (
            reinterpret_cast<char*>(fField.begin()),
            fField.size()*sizeof(scalar)
        );

        os  << std::endl;
    }
    else
    {
        forAll(fField, i)
        {
            os  << fField[i];

            if (i > 0 && (i % 10) == 0)
            {
                os  << std::endl;
            }
            else
            {
                os  << ' ';
            }
        }
        os << std::endl;
    }
}


void Foam::writeFuns::write
(
    std::ostream& os,
    const bool binary,
    DynamicList<scalar>& fField
)
{
    List<scalar>& fld = fField.shrink();

    write(os, binary, fld);
}


void Foam::writeFuns::write
(
    std::ostream& os,
    const bool binary,
    labelList& elems
)
{
    if (binary)
    {
#       ifdef LITTLEENDIAN
        swapWords
        (
            elems.size(),
            reinterpret_cast<char*>(elems.begin()),
            sizeof(label)
        );
#       endif
        os.write
        (
            reinterpret_cast<char*>(elems.begin()),
            elems.size()*sizeof(label)
        );

        os  << std::endl;
    }
    else
    {
        forAll(elems, i)
        {
            os  << elems[i];

            if (i > 0 && (i % 10) == 0)
            {
                os  << std::endl;
            }
            else
            {
                os  << ' ';
            }
        }
        os  << std::endl;
    }
}


void Foam::writeFuns::write
(
    std::ostream& os,
    const bool binary,
    DynamicList<label>& elems
)
{
    labelList& fld = elems.shrink();

    write(os, binary, fld);
}


void Foam::writeFuns::writeHeader
(
    std::ostream& os,
    const bool binary,
    const std::string& title
)
{
    os  << "# vtk DataFile Version 2.0" << std::endl
        << title << std::endl;

    if (binary)
    {
        os  << "BINARY" << std::endl;
    }
    else
    {
        os  << "ASCII" << std::endl;
    }
}


void Foam::writeFuns::writeCellDataHeader
(
    std::ostream& os,
    const label nCells,
    const label nFields
)
{
    os  << "CELL_DATA " << nCells << std::endl
        << "FIELD attributes " << nFields << std::endl;
}


void Foam::writeFuns::writePointDataHeader
(
    std::ostream& os,
    const label nPoints,
    const label nFields
)
{
    os  << "POINT_DATA  " << nPoints << std::endl
        << "FIELD attributes " << nFields << std::endl;
}


void Foam::writeFuns::insert(const scalar src, DynamicList<scalar>& dest)
{
    dest.append(src);
}


void Foam::writeFuns::insert(const vector& src, DynamicList<scalar>& dest)
{
    for (direction cmpt = 0; cmpt < vector::nComponents; ++cmpt)
    {
        dest.append(src[cmpt]);
    }
}


void Foam::writeFuns::insert
(
    const sphericalTensor& src,
    DynamicList<scalar>& dest
)
{
    for (direction cmpt = 0; cmpt < sphericalTensor::nComponents; ++cmpt)
    {
        dest.append(src[cmpt]);
    }
}


void Foam::writeFuns::insert
(
    const symmTensor& src,
    DynamicList<scalar>& dest
)
{
    dest.append(src.xx());
    dest.append(src.yy());
    dest.append(src.zz());
    dest.append(src.xy());
    dest.append(src.yz());
    dest.append(src.xz());
}


void Foam::writeFuns::insert(const tensor& src, DynamicList<scalar>& dest)
{
    for (direction cmpt = 0; cmpt < tensor::nComponents; ++cmpt)
    {
        dest.append(src[cmpt]);
    }
}


void Foam::writeFuns::insert(const labelList& src, DynamicList<label>& dest)
{
    dest.append(src);
}


// ************************************************************************* //

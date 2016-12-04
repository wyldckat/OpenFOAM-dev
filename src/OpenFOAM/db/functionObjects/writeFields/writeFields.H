/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2016 OpenFOAM Foundation
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

Class
    Foam::functionObjects::writeFields

Description
    functionObject base class for managing the list of fields handled locally
    by the derived function object.

See also
    Foam::functionObject

SourceFiles
    writeFields.C

\*---------------------------------------------------------------------------*/

#ifndef functionObjects_writeFields_H
#define functionObjects_writeFields_H

#include "wordList.H"
#include "wordReList.H"
#include "Switch.H"
#include "objectRegistry.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{

/*---------------------------------------------------------------------------*\
                     Class writeFields Declaration
\*---------------------------------------------------------------------------*/

class writeFields
{

protected:

    // Protected data

        //- Parent type name
        const word& typeName_;

        //- Parent name
        const word& name_;

        //- Reference to the region objectRegistry
        const objectRegistry& writeObr_;

        //- Reference to main log switch
        const Switch& writeLog_;

        //- Field names handled by this function object
        wordList localFieldNames_;

        //- Field names to be written
        wordReList writeFieldNames_;

    // Protected Member Functions

        //- Reset the list of local field names from a single word
        virtual void resetLocalFieldName(const word& name);

        //- Reset the list of local field names from a wordList
        virtual void resetLocalFieldNames(const wordList& names);

        //- Get the list of field names to be written
        virtual wordList objectNames();

        //- Write the requested registered IO object
        virtual void writeObject(const regIOobject& obj);

private:

    // Private Member Functions

        //- Disallow default bitwise copy construct
        writeFields(const writeFields&);

        //- Disallow default bitwise assignment
        void operator=(const writeFields&);


public:

    // Constructors

        //- Construct from objectRegistry and prefix
        writeFields
        (
            const objectRegistry& obr,
            const word& name,
            const word& type,
            const Switch& logRef
        );


    //- Destructor
    virtual ~writeFields();


    // Member Functions

        //- Return const access to the local field names
        const wordList& localFieldNames() const;

        //- Return const access to the field names to be written
        const wordReList& writeFieldNames() const;

        //- Read the list of fields to be written
        virtual bool read(const dictionary&);

        //- Write function
        virtual bool write();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace functionObjects
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //

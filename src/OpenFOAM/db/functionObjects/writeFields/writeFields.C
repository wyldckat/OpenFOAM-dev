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

\*---------------------------------------------------------------------------*/

#include "writeFields.H"
#include "Time.H"
#include "stringListOps.H"
#include "dictionary.H"

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

void Foam::functionObjects::writeFields::resetLocalFieldName(const word& name)
{
    localFieldNames_.clear();
    localFieldNames_.append(name);
}


void Foam::functionObjects::writeFields::resetLocalFieldNames
(
    const wordList& names
)
{
    localFieldNames_.clear();
    localFieldNames_.append(names);
}


Foam::wordList Foam::functionObjects::writeFields::objectNames()
{
    wordList names_
    (
        subsetStrings(wordReListMatcher(writeFieldNames_), localFieldNames_)
    );

    return names_;
}


void Foam::functionObjects::writeFields::writeObject(const regIOobject& obj)
{
    if(writeLog_) Info << "    writing field " << obj.name() << endl;

    obj.write();
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjects::writeFields::writeFields
(
    const objectRegistry& obr,
    const word& name,
    const word& typeName,
    const Switch& logRef
)
:
    typeName_(typeName),
    name_(name),
    writeObr_(obr),
    writeLog_(logRef),
    localFieldNames_(),
    writeFieldNames_()
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::functionObjects::writeFields::~writeFields()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::wordList&
Foam::functionObjects::writeFields::localFieldNames() const
{
    return localFieldNames_;
}


const Foam::wordReList&
Foam::functionObjects::writeFields::writeFieldNames() const
{
    return writeFieldNames_;
}


bool Foam::functionObjects::writeFields::read(const dictionary& dict)
{
    if (dict.found("writeFields"))
    {
        dict.lookup("writeFields") >> writeFieldNames_;
    }
    else
    {
        writeFieldNames_.setSize(1, wordRe(".*"));
    }

    return true;
}


bool Foam::functionObjects::writeFields::write()
{
    wordList names(objectNames());

    if(!names.empty())
    {
        if (!writeObr_.time().writeTime())
        {
            writeObr_.time().writeTimeDict();
        }

        forAll(names, i)
        {
            const regIOobject& obj =
                writeObr_.lookupObject<regIOobject>(names[i]);

            writeObject(obj);
        }
    }

    return true;
}


// ************************************************************************* //

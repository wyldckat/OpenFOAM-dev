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

#include "writeObjectsBase.H"
#include "Time.H"
#include "stringListOps.H"
#include "dictionary.H"

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

void Foam::functionObjects::writeObjectsBase::resetLocalObjectName
(
    const word& name
)
{
    localObjectNames_.clear();
    localObjectNames_.append(name);
}


void Foam::functionObjects::writeObjectsBase::resetLocalObjectNames
(
    const wordList& names
)
{
    localObjectNames_.clear();
    localObjectNames_.append(names);
}


Foam::wordList Foam::functionObjects::writeObjectsBase::objectNames()
{
    wordList names_
    (
        subsetStrings(wordReListMatcher(writeObjectNames_), localObjectNames_)
    );

    return names_;
}


void Foam::functionObjects::writeObjectsBase::writeObject
(
    const regIOobject& obj
)
{
    if(funcObj_.log) Info << "    writing field " << obj.name() << endl;

    obj.write();
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjects::writeObjectsBase::writeObjectsBase
(
    const objectRegistry& obr,
    functionObject& obj
)
:
    writeObr_(obr),
    funcObj_(obj),
    localObjectNames_(),
    writeObjectNames_()
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::functionObjects::writeObjectsBase::~writeObjectsBase()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::wordList&
Foam::functionObjects::writeObjectsBase::localObjectNames() const
{
    return localObjectNames_;
}


const Foam::wordReList&
Foam::functionObjects::writeObjectsBase::writeObjectNames() const
{
    return writeObjectNames_;
}


bool Foam::functionObjects::writeObjectsBase::read(const dictionary& dict)
{
    if (dict.found("objects"))
    {
        dict.lookup("objects") >> writeObjectNames_;
    }
    else
    {
        writeObjectNames_.setSize(1, wordRe(".*"));
    }

    return true;
}


bool Foam::functionObjects::writeObjectsBase::write()
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

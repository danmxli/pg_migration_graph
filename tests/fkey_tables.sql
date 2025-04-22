CREATE TABLE Persons (
    PersonID int,
    LastName varchar(255),
    FirstName varchar(255),
    Address varchar(255),
    City varchar(255)
);

CREATE TABLE Dogs (
    DogID int,
    Name varchar(255),
    Breed varchar(255),
    Age int,
    OwnerID int,
    PRIMARY KEY (DogID),
    FOREIGN KEY (OwnerID) REFERENCES Persons(PersonID)
);
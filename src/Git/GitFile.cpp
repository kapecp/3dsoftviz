#include "Git/GitFile.h"
#include "Git/GitFileDiffBlock.h"

#include <QDebug>

Git::GitFile::GitFile() {

    // inicializacia premennych na defaultne hodnoty
    this->filename = "";
    this->filepath = "";
    this->type = Git::GitType::NONE;
    this->gitFileDiffBlocks = QList<Git::GitFileDiffBlock*>();
}

Git::GitFile::GitFile(QString filename, QString filepath, GitType type) : filename(filename), filepath(filepath), type(type) {
    this->gitFileDiffBlocks = QList<Git::GitFileDiffBlock*>();
}

QString Git::GitFile::getTypeAsString() {

    // zoberie typ suboru a vrati ho ako QString
    switch (this->getType()) {
    case Git::GitType::ADDED:
        return "Added";
    case Git::GitType::MODIFIED:
        return "Modified";
    case Git::GitType::REMOVED:
        return "Removed";
    default:
        return "Wrong or Not set";
    }
}

void Git::GitFile::printContent() {

    // vypise informacie o subore
    qDebug() << "filename: " << getFilename();
    qDebug() << "filepath: " << getFilepath();
    qDebug() << "type: " << getTypeAsString();

    // pre kazdy diff blok vypis informacie o danom bloku
    foreach (GitFileDiffBlock* block, getGitFileDiffBlocks()) {
        block->printInfo();
    }
}

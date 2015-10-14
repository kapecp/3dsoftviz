#include "Git/GitFile.h"
#include "Git/GitFileDiffBlock.h"

#include <QDebug>

Git::GitFile::GitFile() {
    this->gitFileDiffBlocks = QList<Git::GitFileDiffBlock*>();
}

Git::GitFile::GitFile(QString filename, QString filepath, GitType type) : filename(filename), filepath(filepath), type(type) {
    this->gitFileDiffBlocks = QList<Git::GitFileDiffBlock*>();
}

QString Git::GitFile::getTypeAsString() {
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
    qDebug() << "filename: " << getFilename();
    qDebug() << "filepath: " << getFilepath();
    qDebug() << "type: " << getTypeAsString();
    foreach (GitFileDiffBlock* block, getGitFileDiffBlocks()) {
        block->printInfo();
    }
}

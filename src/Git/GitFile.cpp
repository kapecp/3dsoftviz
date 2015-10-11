#include "Git/GitFile.h"

Git::GitFile::GitFile() {}

Git::GitFile::GitFile(QString filename, QString filepath, GitFileType type) : filename(filename), filepath(filepath), type(type) {

}

QString Git::GitFile::getTypeString() {
    switch (this->getType()) {
    case Git::GitFileType::ADDED:
        return "Added";
    case Git::GitFileType::MODIFIED:
        return "Modified";
    case Git::GitFileType::REMOVED:
        return "Removed";
    default:
        return "Not set";
    }
}

#include "Repository/Git/GitLuaGraphAnalyzer.h"

#include "LuaGraph/LuaGraph.h"
#include "LuaGraph/LuaNode.h"
#include "LuaGraph/LuaEdge.h"
#include "LuaGraph/LuaIncidence.h"

#include "GitLib/GitVersion.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitFunction.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitUtils.h"

#include <QDebug>
#include <QList>

Repository::Git::GitLuaGraphAnalyzer::GitLuaGraphAnalyzer()
    : luaGraph( Lua::LuaGraph::getInstance() ), evolutionGraph( nullptr ), versionNumber( -1 ), functions( new QMap<QString, Repository::Git::GitFunction*>() )
{
}

Repository::Git::GitLuaGraphAnalyzer::GitLuaGraphAnalyzer( Lua::LuaGraph *luaGraph, Repository::Git::GitEvolutionGraph *evolutionGraph )
    : luaGraph( luaGraph ), evolutionGraph( evolutionGraph ), versionNumber( -1 ), functions( new QMap<QString, Repository::Git::GitFunction*>() )
{

}

Repository::Git::GitLuaGraphAnalyzer::~GitLuaGraphAnalyzer() {
    qDebug() << "Destruktor GitLuaGraphAnalyzer";
    delete functions;
}

void Repository::Git::GitLuaGraphAnalyzer::analyze() {
    Repository::Git::GitVersion* version =  this->evolutionGraph->getVersion( this->versionNumber );
    QMap<QString, Repository::Git::GitFile*> versionFiles = version->getChangedFiles();

    for( QMap<qlonglong, Lua::LuaNode*>::iterator iterator = this->luaGraph->getNodes()->begin(); iterator != this->luaGraph->getNodes()->end(); ++iterator ) {
        if( versionFiles.contains( iterator.value()->getIdentifier()  ) ) {
            Lua::LuaNode* node = iterator.value();

            QMap<QString, Repository::Git::GitFunction*> functions = QMap<QString, Repository::Git::GitFunction*>();

            Repository::Git::GitFile* file = new Repository::Git::GitFile( node->getIdentifier(), node->getIdentifier().split(":").at( 1 ), Repository::Git::GitType::MODIFIED );

            int counter = 0;
            foreach( qlonglong incidenceId, node->getIncidences() ) {
                Lua::LuaIncidence* incidence = this->luaGraph->getIncidences()->value( incidenceId );

                // Ak je incidence orientovany a outGoing je false, tak ide o hranu zo suboru na globalFunction
                // a pridam k danemu suboru vsetky global funkcie spolu s ich modulmi
                if( incidence->getOriented() && !incidence->getOutGoing() ) {
                    Lua::LuaEdge* edge = this->luaGraph->getEdges()->value( incidence->getEdgeNodePair().first );
                    Lua::LuaNode* pairNode = nullptr;

                    Lua::LuaIncidence* otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 0 ) );
                    if( incidence == otherIncidence ) {
                        otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 1 ) );
                    }

                    pairNode = this->luaGraph->getNodes()->value( otherIncidence->getEdgeNodePair().second );

                    QString functionIdentifier =  edge->getIdentifier().replace( "+", "" ).replace( node->getIdentifier(), "" );
//                    qDebug() << "Identifikator" << functionIdentifier;

                    Repository::Git::GitFunction* function = functions.value( functionIdentifier );

                    if( !function ) {

                        function = new Repository::Git::GitFunction();
                        QStringList list = functionIdentifier.split( ":" );

                        // Ak sa pocet tokenov z identifikatora rovna 2, tak globalna funkcia nema global modul
                        // inak nenastavim modul
                        if( list.size() == 2 ) {
                            function->setName( list.at( 1 ) );
                        } else {
                            function->setName( list.at( 2 ) );
                            function->setModule( list.at( 1 ) );
                        }

                        function->setType( Repository::Git::GitType::NONE );
                        function->setId( pairNode->getId() );

                        if( !QString::compare( QString::fromStdString( pairNode->getParams()["type"].asString() ), "globalFunction" ) ) {
                            function->setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );
                        } else {
                            qDebug() << "CHYBA V MOJEJ HLAVE!!!!! pri dedikcii globalnej funckii";
                        }

                        functions.insert( functionIdentifier, function );
                    }

                    file->addGitFunction( function );
                    counter++;

                } else {
                    //ToDo Tie, ktore nie su orientovane, treba skontrolovat dalsi edge a druhy node, ak je rovny function tak ho dalej spracuj, inak nespracuj dany node
                    Lua::LuaEdge* edge = this->luaGraph->getEdges()->value( incidence->getEdgeNodePair().first );
                    Lua::LuaNode* pairNode = nullptr;

                    Lua::LuaIncidence* otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 0 ) );
                    if( incidence == otherIncidence ) {
                        otherIncidence = this->luaGraph->getIncidences()->value( edge->getIncidences().at( 1 ) );
                    }

                    pairNode = this->luaGraph->getNodes()->value( otherIncidence->getEdgeNodePair().second );

                    // Pokracujeme v spracovani len ak ide o hranu s lokalnou funkciou
                    if( !QString::compare( QString::fromStdString( pairNode->getParams()["type"].asString() ), "function") ) {
                        QString functionIdentifier = edge->getIdentifier().replace( "+", "" ).replace( node->getIdentifier(), "" );

                        Repository::Git::GitFunction* function =  functions.value( functionIdentifier );

                        if( !function ) {
                            function = new Repository::Git::GitFunction();

                            QStringList list = functionIdentifier.split( ":" );

                            function->setName( list.at( 2 ) );

                            function->setType( Repository::Git::GitType::NONE );
                            function->setId( pairNode->getId() );


                            function->setFunctionType( Repository::Git::GitFunctionType::LOCALFUNCTION );
                            functions.insert( functionIdentifier, function );
                        }

                        file->addGitFunction( function );

                        foreach( qlonglong functionIncidenceId, pairNode->getIncidences() ) {
                            Lua::LuaIncidence* functionIncidence = this->luaGraph->getIncidences()->value( functionIncidenceId );

                            if( functionIncidence->getOriented() && !functionIncidence->getOutGoing() ) {
                                Lua::LuaEdge* functionEdge = this->luaGraph->getEdges()->value( functionIncidence->getEdgeNodePair().first );
                                Lua::LuaNode* functionPairNode = nullptr;

                                Lua::LuaIncidence* functionOtherIncidence = this->luaGraph->getIncidences()->value( functionEdge->getIncidences().at( 0 ) );

                                if( functionIncidence == functionOtherIncidence ) {
                                    functionOtherIncidence = this->luaGraph->getIncidences()->value( functionEdge->getIncidences().at( 1 ) );
                                }

                                functionPairNode = this->luaGraph->getNodes()->value( functionOtherIncidence->getEdgeNodePair().second );

                                QString innerFunctionIdentifier =  functionEdge->getIdentifier().replace( "+", "" ).replace( pairNode->getIdentifier(), "" );
//                                qDebug() << "Vnoreny Identifier" << innerFunctionIdentifier << "v" << functionIdentifier ;

                                Repository::Git::GitFunction* innerFunction = functions.value( innerFunctionIdentifier );

                                if( !innerFunction ) {
                                    innerFunction = new Repository::Git::GitFunction();
                                    QStringList list = innerFunctionIdentifier.split( ":" );

                                    if( !QString::compare( QString::fromStdString( functionPairNode->getParams()["type"].asString() ), "function" ) ) {
                                        innerFunction->setName( list.at( 2 ) );
                                        innerFunction->setFunctionType( Repository::Git::GitFunctionType::LOCALFUNCTION );
                                    } else {
                                        if( list.size() == 2 ) {
                                            innerFunction->setName( list.at( 1 ) );
                                        } else {
                                            innerFunction->setName( list.at( 2 ) );
                                            innerFunction->setModule( list.at( 1 ) );
                                        }
                                        innerFunction->setFunctionType( Repository::Git::GitFunctionType::GLOBALFUNCTION );
                                    }
                                    innerFunction->setId( functionPairNode->getId() );
                                    innerFunction->setType( Repository::Git::GitType::NONE );
                                }

                                functions.insert( innerFunctionIdentifier, innerFunction );
                                function->addFunctionCaller( innerFunction );
                                innerFunction->addFunctionCallee( function );
                            }
                        }
                    }

                }

            }


            if( evolutionGraph->getLatestGitFileCallTree().contains( file->getIndetifier() ) ) {
                qDebug() << versionNumber << "obsahuje" << file->getIndetifier();

                //ToDo Pridat metodu, ktora porovna sucasny file s ulozenym file a uloz9 ich rozdiel do evolutionGraph
            } else {
                qDebug() << versionNumber << "neobsahuje" << file->getIndetifier();
                evolutionGraph->addLatestGitFileCallTree( file->getIndetifier(), file );
            }

/*
            for( QMap<QString, Repository::Git::GitFunction*>::iterator  iterator = file->getGitFunctions()->begin(); iterator != file->getGitFunctions()->end(); ++iterator ) {
                Repository::Git::GitFunction* function = iterator.value();

                qDebug() << file->getIndetifier() << "->" <<  function->getIdentifier();

                for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = function->getFunctionCallers()->begin(); iter != function->getFunctionCallers()->end(); ++iter ) {
                    Repository::Git::GitFunction* innerFunction = iter.value();
                    qDebug() << "Caller ->" << innerFunction->getIdentifier();
                }

                for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = function->getFunctionCallees()->begin(); iter != function->getFunctionCallees()->end(); ++iter ) {
                    Repository::Git::GitFunction* innerFunction = iter.value();
                    qDebug() << "Callee ->" << innerFunction->getIdentifier();
                }
            }
*/
            qDebug() << node->getIdentifier() << "s poctom incidence" << node->getIncidences().size() << "/" << counter << "/" << file->getGitFunctions()->size();
        }
    }

//    Repository::Git::GitUtils::getModifiedLuaNodesFromVersion( this->evolutionGraph, this->versionNumber );
    version->setIsLoaded( true );

}



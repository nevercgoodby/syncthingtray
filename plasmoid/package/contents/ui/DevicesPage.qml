import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQml.Models 2.2
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras

Item {
    property alias view: deviceView
    anchors.fill: parent
    objectName: "DevicesPage"

    PlasmaExtras.ScrollArea {
        anchors.fill: parent

        TopLevelView {
            id: deviceView
            model: plasmoid.nativeInterface.devModel

            delegate: TopLevelItem {
                id: item

                ColumnLayout {
                    width: parent.width
                    spacing: 0

                    RowLayout {
                        RowLayout {
                            spacing: 5
                            PlasmaCore.IconItem {
                                Layout.preferredWidth: 16
                                Layout.preferredHeight: 16
                                anchors.verticalCenter: parent.verticalCenter
                                source: statusIcon
                            }
                            PlasmaComponents.Label {
                                anchors.verticalCenter: parent.verticalCenter
                                elide: Text.ElideRight
                                text: name
                            }
                        }
                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                        RowLayout {
                            id: toolButtonsLayout
                            spacing: 0

                            PlasmaComponents.Label {
                                height: implicitHeight
                                text: statusString
                                color: statusColor
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Item {
                                width: 3
                            }
                            PlasmaComponents.ToolButton {
                                id: barcodeToolButton
                                iconSource: paused ? "media-playback-start" : "media-playback-pause";
                                tooltip: paused ? qsTr("Resume") : qsTr("Pause")
                                onClicked: paused
                                           ? plasmoid.nativeInterface.connection.resumeDevice([devId])
                                           : plasmoid.nativeInterface.connection.pauseDevice([devId])
                            }
                        }
                    }

                    DetailView {
                        id: detailsView
                        visible: item.expanded

                        model: DelegateModel {
                            model: plasmoid.nativeInterface.devModel
                            rootIndex: detailsView.model.modelIndex(index)
                            delegate: DetailItem { }
                        }
                    }

                }
            }
        }
    }
}
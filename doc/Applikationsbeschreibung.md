<!-- SPDX-License-Identifier: AGPL-3.0-only -->
<!-- Copyright (C) 2025 Andreas   Michael Geramb -->

# Applikationsbeschreibung Fingerprint

Die Applikation StoveControl erlaubt eine Parametrisierung eines KNX 2 Fumis Gateways mit der ETS.

## Wichtige Hinweise

* Diese KNXprod wird nicht von der KNX Association offiziell unterstützt!
* Die Erzeugung der KNXprod geschieht auf Eure eigene Verantwortung!

## **Verwendete Module**

Der StoveControl verwendet weitere OpenKNX-Module, die alle ihre eigene Dokumentation besitzen. Im folgenden werden die Module und die Verweise auf deren Dokumentation aufgelistet.

### **OpenKNX**

Dies ist eine Seite mit allgemeinen Parametern, die unter [Applikationsbeschreibung-Common](https://github.com/OpenKNX/OGM-Common/blob/v1/doc/Applikationsbeschreibung-Common.md) beschrieben sind. 

### **Konfigurationstransfer**

Der Konfigurationstransfer erlaubt einen

* Export von Konfigurationen von OpenKNX-Modulen und deren Kanälen
* Import von Konfigurationen von OpenKNX-Modulen und deren Kanälen
* Kopieren der Konfiguration von einem OpenKNX-Modulkanal auf einen anderen
* Zurücksetzen der Konfiguration eines OpenKNX-Modulkanals auf Standardwerte

Die Funktionen vom Konfigurationstranfer-Modul sind unter [Applikationsbeschreibung-ConfigTransfer](https://github.com/OpenKNX/OFM-ConfigTransfer/blob/v1/doc/Applikationsbeschreibung-ConfigTransfer.md) beschrieben.

### **Logiken**

Wie die meisten OpenKNX-Applikationen enthält auch die StoveControl-Applikation ein Logikmodul.

Die Funktionen des Logikmoduls sind unter [Applikationsbeschreibung-Logik](https://github.com/OpenKNX/OFM-LogicModule/blob/v1/doc/Applikationsbeschreibung-Logik.md) beschrieben.

### **Sensore**

Durch die Einbindung des SensorModules werden einen vielzahl von i2c Sensoren automatisch unterstütz.

Die Funktionen des Sensormoduls sind unter [Applikationsbeschreibung-Sesnore](https://github.com/OpenKNX/OFM-SensorModule/blob/v1/doc/Applikationsbeschreibung-Sensor.md) beschrieben.


## **StoveControl**

<!-- DOC HelpContext="Dokumentation" -->
Mit diesem Modul können Daten aus dem Fumis Controller im Pelletofen ausgelsen und geschrieben werden.

<!-- DOC -->
### Anzeige ausschalten nach

<!-- DOC -->
#### Anzeige aus nach Zeit

<!-- DOC -->
#### Anzeige aus nach Zeitbasis

<!-- DOC -->
### Standardseite Rückfallzeit 

<!-- DOC -->
#### Standardseite nach Zeit 

<!-- DOC -->
#### Standardseite nach Zeitbasis 

<!-- DOC -->
### Seite über Blaettern erreichbar

<!-- DOC -->
### Absprung zu

<!-- DOC -->
### Feldanzahl


<!-- DOC -->
### Felddefintion

<!-- DOC -->
### Funktion

<!-- DOC -->
### Kurzer Druck

<!-- DOC -->
### Langer Druck


ToDo



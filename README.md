# 🕹️ AI Steering & Rescuer Lab

> **Simulation de comportement directionnel autonome d'agents dans Unreal Engine en C++.**

## 📜 Table des matières

1. **[📌 Introduction](#-introduction)**
1. **[🕹️ Lab 1: Lab 1 : Steering Behaviors](#️-lab-1--steering-behaviors)**
   1. [🎯 Objectif](#-objectif)
   1. [🧠 Comportements implémentés](#-comportements-implémentés)
1. **[🚑 Lab 2 : Rescuer Simulation](#-lab-2--rescuer-simulation)**
   1. [🎯 Objectif](#-objectif-rescuer)
   1. [🧠 Comportements implémentés](#-comportements-et-algorithmes-encours-dimplémentation)
1. **[💾 Installation](#-installation)**
1. **[👨‍💻 Auteur](#-auteur)**

## 📌 Introduction

Ce projet Unreal Engine regroupe **deux simulations** basées sur des comportements autonomes en intelligence artificielle :

1. **Steering Lab** : Implémente des comportements de steering (Seek, Flee, Pursuit, etc.) pour agents autonomes.
1. **Rescuer Lab** : Simule une ville où des agents NPC et un joueur sauvent des victimes en appliquant du pathfinding et du planning.

Les deux projets sont inclus dans **une seule release**.

## 🕹️ Lab 1 : Steering Behaviors

### 🎯 Objectif

L’objectif de ce lab est d’implémenter une "**steering library**" permettant aux agents autonomes de se déplacer de manière réaliste.

> **📌 Vidéo de référence :** [AI In Games - Steering Behaviours: Intro](https://www.youtube.com/watch?v=j7IzUToM0eY)

### 🧠 Comportements implémentés

#### 1️⃣ Comportements de base

- **✅ Seek** 🏃‍♂️➡️ : Se dirige vers une cible fixe.
- **✅ Flee** 🏃‍♂️⬅️ : Fuit une cible fixe.
- **✅ Pursuit** 🎯 : Intercepte une cible en mouvement en prédisant sa position future.
- **✅ Evade** 🚫 : Fuit un poursuivant en prédisant sa trajectoire.
- **✅ Arrival** 🏁 : Se dirige vers une cible puis ralentit progressivement avant d’arriver.

#### 2️⃣ Comportements combinés

- **✅ Circuit** 🔄 : Suit un chemin en boucle infinie.
- **✅ One Way** ➡️ : Suit un chemin jusqu’à un point donné, puis s’arrête.
- **✅ Two Ways** ⬅️➡️ : Suit un chemin jusqu’à un point donné, puis fait demi-tour et revient.

## 🚑 Lab 2 : Rescuer Simulation

### 🎯 Objectif Rescuer

Ce lab simule une **opération de sauvetage** dans une ville où des **NPCs doivent secourir** des victimes et les **emmener** à l’hôpital.

**📌 Les NPCs doivent agir intelligemment et optimiser leur comportement.**

### 🧠 Comportements et algorithmes encours d'implémentation

#### 1️⃣ Planification et pathfinding

- **✅ Un planner** choisit la victime à sauver en priorité.
- **✅ Un pathfinding** algorithm (Astar) trouve le chemin optimal.

#### 2️⃣ Déplacement et évitement

- **✅ Mouvement** basé sur le **Steering Lab** (Seek, Arrival, etc.).
- **✅ Évitement** de collisions avec d’autres NPCs et victimes.
- **✅ Navigation fluide**.

## 💾 Installation

### 1️⃣ Télécharger la release

1. Allez sur la page **GitHub Releases** : **[📥 Télécharger ici.](https://github.com/vraus/Steering_Lab/releases)**
1. **Téléchargez** le fichier `.zip` de la Release la plus récente.
1. **Décompressez** le fichier.

### 2️⃣ Lancer le jeu

1. **Ouvrez** le dossier extrait.
1. **Lancez** le fichier `.exe`.
1. **Les deux labs sont inclus dans l’exécutable** : vous pourrez choisir le mode souhaité dans le menu principal.
1. **Suivez le tutoriel** in-game qui vous guidera dans l'utilisation de la simulation.

## 👨‍💻 Auteur

_[Vraus](https://github.com/vraus)_

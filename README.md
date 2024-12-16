# Tomeo Video Player 🎥

A lightweight, cross-platform video player built with Qt framework that automatically detects and plays local video files.

## ✨ Features

### Core Functionality
- Cross-platform support (Windows, macOS, Linux)
- Auto-detection and playback of local video files
- Automatic thumbnail generation for playlist
- Real-time video preview in playlist

### Video Playback
- Auto-play first video on launch
- Fluid video navigation controls
- Frame-accurate playback
- Support for multiple video formats

### User Interface
- Clean, modern interface
- Customizable playback controls
- Easy-to-use playlist management 
- Video progress visualization

### Controls
- Previous/Next video navigation
- 2-second forward/backward skip
- Play/Pause toggle
- Timeline scrubbing
- Volume control with mute option
- Playback speed adjustment

## 🚀 Getting Started

### Prerequisites

- Qt 5.x or higher with the following modules:
 - core
 - gui
 - widgets
 - multimedia
 - multimediawidgets
- C++11 compatible compiler

## 💻 Usage

### Supported Video Formats
- Windows: .wmv
- macOS/Linux: .mov / .mp4

### Control Guide

#### Basic Controls
- **Play/Pause**: <img src="icons/play-dark.svg" alt="play icon" width="16" height="16" style="vertical-align: middle;"> Play button 
- **Skip Forward**: <img src="icons/fast-forward-dark.svg" alt="play icon" width="16" height="16" style="vertical-align: middle;"> Forward button (2 seconds)
- **Skip Backward**: <img src="icons/rewind-dark.svg" alt="play icon" width="16" height="16" style="vertical-align: middle;"> Backward button (2 seconds)
- **Next Video**: <img src="icons/next-dark.svg" alt="play icon" width="16" height="16" style="vertical-align: middle;"> Next button
- **Previous Video**: <img src="icons/previous-dark.svg" alt="play icon" width="16" height="16" style="vertical-align: middle;"> Previous button

#### Advanced Controls
- **Volume**:  Slider or mouse wheel over volume area 
- **Mute/Unmute**:  Click speaker icon <img src="icons/volume.svg" alt="play icon" width="16" height="16" style="vertical-align: middle;"> / <img src="icons/mute.svg" alt="play icon" width="16" height="16" style="vertical-align: middle;">
- **Timeline**: Drag progress bar
- **Speed Control**: Adjust using speed button

#### Playlist
- Automatically displays all compatible videos
- Click thumbnail to play selected video

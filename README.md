# Tomeo Video Player 🎥

A lightweight, cross-platform video player built with Qt framework that automatically detects and plays local video files.

## ✨ Features

### Core Functionality
- Cross-platform support (Windows, macOS, Linux)
- Auto-detection and playback of local video files
- Automatic thumbnail generation for playlist
- Real-time video preview in playlist
- Read Delete Target Video

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
- Add videos to videolist
- Delete videolist
- Remove single video from videolist

## 🚀 Getting Started

### Prerequisites
- Qt 5.x or higher
- C++11 compatible compiler
- Qt Multimedia module
- Qt Widgets module

### Installation

#### Building from Source

1. Clone the repository from GitHub
2. Navigate to the project directory
3. Create and enter build directory
4. Generate build files with qmake
5. Build the project using your system's compiler

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
- **Mute/Unmute**:  Click speaker icon <img src="icons/volume-dark.svg" alt="play icon" width="16" height="16" style="vertical-align: middle;"> / <img src="icons/mute-dark.svg" alt="play icon" width="16" height="16" style="vertical-align: middle;">
- **Timeline**: Drag progress bar
- **Speed Control**: Adjust using speed button

#### Playlist
- Automatically displays all compatible videos
- Click thumbnail to play selected video

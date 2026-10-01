# Architecture Overview

Ellindyer World Forge is a modular desktop application.

## Layers

    UI
     ↓
    Application
     ↓
    Domain
     ↓
    Infrastructure

## Principles

- The application is the product.
- UI and backend grow together.
- Use the C++ standard library and the Windows SDK wherever sufficient.
- Do not build custom replacements for existing facilities.
- Do not build a game engine.
- Every phase must build and run.

# Resources

## Layout

    resources/
    ├── icons/
    │   └── app_logo.png
    ├── fonts/
    │   └── Roboto/
    │       ├── Roboto-Regular.ttf
    │       ├── Roboto-Medium.ttf
    │       ├── Roboto-Bold.ttf
    │       └── Roboto-Light.ttf
    └── themes/

## Rules

- The only image asset is `icons/app_logo.png`.
- The only font asset family is Roboto.
- The UI is text-based and monochrome (black and white).
- Icons use OS system icons or Unicode emoji.
- No other binary assets.

## Splash Screen Usage

- `icons/app_logo.png` is rendered on the splash screen.
- `fonts/Roboto/*.ttf` are loaded into Dear ImGui and used for all UI text.
- Missing assets fall back gracefully: the splash screen renders a textual
  placeholder if the logo is absent, and Dear ImGui's built-in font is used
  when Roboto files are absent.

#pragma once

#include "TermControl.g.h"
#include <winrt/Microsoft.Terminal.TerminalConnection.h>
#include <winrt/Microsoft.Terminal.Settings.h>
#include "../../renderer/base/Renderer.hpp"
#include "../../renderer/dx/DxRenderer.hpp"
#include "../../cascadia/TerminalCore/Terminal.hpp"
#include "../../cascadia/inc/cppwinrt_utils.h"

namespace winrt::Microsoft::Terminal::TerminalControl::implementation
{
    struct TermControl : TermControlT<TermControl>
    {
        TermControl();
        TermControl(Settings::IControlSettings settings);

        Windows::UI::Xaml::UIElement GetRoot();
        Windows::UI::Xaml::Controls::UserControl GetControl();
        Settings::IControlSettings GetSettings();
        void UpdateSettings(Settings::IControlSettings newSettings);

        hstring Title();
        void CopySelectionToClipboard();
        void Close();

        void ScrollViewport(int viewTop);
        int GetScrollOffset();

        void SwapChainChanged();
        ~TermControl();

        static Windows::Foundation::Point GetProposedDimensions(Microsoft::Terminal::Settings::IControlSettings const& settings, const uint32_t dpi);

        // -------------------------------- WinRT Events ---------------------------------
        DECLARE_EVENT(TitleChanged,             _titleChangedHandlers,              TerminalControl::TitleChangedEventArgs);
        DECLARE_EVENT(ConnectionClosed,         _connectionClosedHandlers,          TerminalControl::ConnectionClosedEventArgs);
        DECLARE_EVENT(ScrollPositionChanged,    _scrollPositionChangedHandlers,     TerminalControl::ScrollPositionChangedEventArgs);
        DECLARE_EVENT(CopyToClipboard,          _clipboardCopyHandlers,             TerminalControl::CopyToClipboardEventArgs);

    private:
        TerminalConnection::ITerminalConnection _connection;
        bool _initializedTerminal;

        Windows::UI::Xaml::Controls::UserControl _controlRoot;
        Windows::UI::Xaml::Controls::Grid _root;
        Windows::UI::Xaml::Controls::SwapChainPanel _swapChainPanel;
        Windows::UI::Xaml::Controls::Primitives::ScrollBar _scrollBar;
        event_token _connectionOutputEventToken;

        ::Microsoft::Terminal::Core::Terminal* _terminal;

        std::unique_ptr<::Microsoft::Console::Render::Renderer> _renderer;
        std::unique_ptr<::Microsoft::Console::Render::DxEngine> _renderEngine;

        Settings::IControlSettings _settings;
        bool _closing;

        FontInfoDesired _desiredFont;
        FontInfo _actualFont;

        std::optional<int> _lastScrollOffset;

        // storage location for the leading surrogate of a utf-16 surrogate pair
        std::optional<wchar_t> _leadingSurrogate;

        // If this is set, then we assume we are in the middle of panning the
        //      viewport via touch input.
        std::optional<winrt::Windows::Foundation::Point> _touchAnchor;

        void _Create();
        void _ApplyUISettings();
        void _ApplyConnectionSettings();
        void _InitializeTerminal();
        void _UpdateFont();
        void _KeyHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void _CharacterHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::CharacterReceivedRoutedEventArgs const& e);
        void _MouseClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void _MouseMovedHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void _PointerReleasedHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void _MouseWheelHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void _ScrollbarChangeHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);

        void _SendInputToConnection(const std::wstring& wstr);
        void _SwapChainSizeChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::SizeChangedEventArgs const& e);
        void _SwapChainScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel const& sender, Windows::Foundation::IInspectable const& args);
        void _DoResize(const double newWidth, const double newHeight);
        void _TerminalTitleChanged(const std::wstring_view& wstr);
        void _TerminalScrollPositionChanged(const int viewTop, const int viewHeight, const int bufferSize);

        void _ScrollbarUpdater(Windows::UI::Xaml::Controls::Primitives::ScrollBar scrollbar, const int viewTop, const int viewHeight, const int bufferSize);
        Windows::UI::Xaml::Thickness _ParseThicknessFromPadding(const hstring padding);
    };
}

namespace winrt::Microsoft::Terminal::TerminalControl::factory_implementation
{
    struct TermControl : TermControlT<TermControl, implementation::TermControl>
    {
    };
}

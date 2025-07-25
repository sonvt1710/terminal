// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "Pane.h"
#include "ColorPickupFlyout.h"
#include "Tab.h"
#include "Tab.g.h"

// fwdecl unittest classes
namespace TerminalAppLocalTests
{
    class TabTests;
};

namespace winrt::TerminalApp::implementation
{
    struct Tab : TabT<Tab>
    {
    public:
        Tab(std::shared_ptr<Pane> rootPane);

        // Called after construction to perform the necessary setup, which relies on weak_ptr
        void Initialize();

        winrt::Microsoft::Terminal::Control::TermControl GetActiveTerminalControl() const;
        winrt::Microsoft::Terminal::Settings::Model::Profile GetFocusedProfile() const noexcept;
        winrt::TerminalApp::IPaneContent GetActiveContent() const;

        void Focus(winrt::Windows::UI::Xaml::FocusState focusState);

        void Scroll(const int delta);

        std::shared_ptr<Pane> DetachRoot();
        std::shared_ptr<Pane> DetachPane();
        void AttachPane(std::shared_ptr<Pane> pane);

        void AttachColorPicker(winrt::TerminalApp::ColorPickupFlyout& colorPicker);

        std::pair<std::shared_ptr<Pane>, std::shared_ptr<Pane>> SplitPane(winrt::Microsoft::Terminal::Settings::Model::SplitDirection splitType,
                                                                          const float splitSize,
                                                                          std::shared_ptr<Pane> newPane);

        void ToggleSplitOrientation();
        void UpdateIcon(const winrt::hstring& iconPath, const winrt::Microsoft::Terminal::Settings::Model::IconStyle iconStyle);
        void HideIcon(const bool hide);

        void ShowBellIndicator(const bool show);
        void ActivateBellIndicatorTimer();

        float CalcSnappedDimension(const bool widthOrHeight, const float dimension) const;
        std::optional<winrt::Microsoft::Terminal::Settings::Model::SplitDirection> PreCalculateCanSplit(winrt::Microsoft::Terminal::Settings::Model::SplitDirection splitType,
                                                                                                        const float splitSize,
                                                                                                        winrt::Windows::Foundation::Size availableSpace) const;

        void ResizePane(const winrt::Microsoft::Terminal::Settings::Model::ResizeDirection& direction);
        bool NavigateFocus(const winrt::Microsoft::Terminal::Settings::Model::FocusDirection& direction);
        bool SwapPane(const winrt::Microsoft::Terminal::Settings::Model::FocusDirection& direction);
        bool FocusPane(const uint32_t id);

        void UpdateSettings(const winrt::Microsoft::Terminal::Settings::Model::CascadiaSettings& settings);
        void UpdateTitle();

        void Shutdown();
        void ClosePane();

        void SetTabText(winrt::hstring title);
        winrt::hstring GetTabText() const;
        void ResetTabText();
        void ActivateTabRenamer();

        std::optional<winrt::Windows::UI::Color> GetTabColor();
        void SetRuntimeTabColor(const winrt::Windows::UI::Color& color);
        void ResetRuntimeTabColor();

        void UpdateZoom(std::shared_ptr<Pane> newFocus);
        void ToggleZoom();
        bool IsZoomed();
        void EnterZoom();
        void ExitZoom();

        std::vector<Microsoft::Terminal::Settings::Model::ActionAndArgs> BuildStartupActions(BuildStartupKind kind) const;

        int GetLeafPaneCount() const noexcept;

        void TogglePaneReadOnly();
        void SetPaneReadOnly(const bool readOnlyState);
        void ToggleBroadcastInput();

        std::shared_ptr<Pane> GetActivePane() const;
        winrt::TerminalApp::TaskbarState GetCombinedTaskbarState() const;

        std::shared_ptr<Pane> GetRootPane() const { return _rootPane; }
        std::vector<uint32_t> GetMruPanes() const { return _mruPanes; }

        winrt::TerminalApp::TerminalTabStatus TabStatus()
        {
            return _tabStatus;
        }

        void SetDispatch(const winrt::TerminalApp::ShortcutActionDispatch& dispatch);

        void UpdateTabViewIndex(const uint32_t idx, const uint32_t numTabs);
        void SetActionMap(const Microsoft::Terminal::Settings::Model::IActionMapView& actionMap);

        void ThemeColor(const winrt::Microsoft::Terminal::Settings::Model::ThemeColor& focused,
                        const winrt::Microsoft::Terminal::Settings::Model::ThemeColor& unfocused,
                        const til::color& tabRowColor);

        Microsoft::Terminal::Settings::Model::TabCloseButtonVisibility CloseButtonVisibility();
        void CloseButtonVisibility(Microsoft::Terminal::Settings::Model::TabCloseButtonVisibility visible);

        til::event<winrt::delegate<void()>> RequestFocusActiveControl;

        til::event<winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>> Closed;
        til::event<winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>> CloseRequested;
        til::property_changed_event PropertyChanged;

        til::typed_event<TerminalApp::TerminalPaneContent> RestartTerminalRequested;

        til::typed_event<TerminalApp::Tab, IInspectable> ActivePaneChanged;
        til::event<winrt::delegate<>> TabRaiseVisualBell;
        til::typed_event<IInspectable, IInspectable> TaskbarProgressChanged;

        // The TabViewIndex is the index this Tab object resides in TerminalPage's _tabs vector.
        WINRT_PROPERTY(uint32_t, TabViewIndex, 0);
        // The TabViewNumTabs is the number of Tab objects in TerminalPage's _tabs vector.
        WINRT_PROPERTY(uint32_t, TabViewNumTabs, 0);

        WINRT_OBSERVABLE_PROPERTY(winrt::hstring, Title, PropertyChanged.raise);
        WINRT_OBSERVABLE_PROPERTY(winrt::hstring, Icon, PropertyChanged.raise);
        WINRT_OBSERVABLE_PROPERTY(bool, ReadOnly, PropertyChanged.raise, false);
        WINRT_PROPERTY(winrt::Microsoft::UI::Xaml::Controls::TabViewItem, TabViewItem, nullptr);

        WINRT_OBSERVABLE_PROPERTY(winrt::Windows::UI::Xaml::FrameworkElement, Content, PropertyChanged.raise, nullptr);

    private:
        static constexpr double HeaderRenameBoxWidthDefault{ 165 };
        static constexpr double HeaderRenameBoxWidthTitleLength{ std::numeric_limits<double>::infinity() };

        winrt::Windows::UI::Xaml::FocusState _focusState{ winrt::Windows::UI::Xaml::FocusState::Unfocused };
        winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem _closeOtherTabsMenuItem{};
        winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem _closeTabsAfterMenuItem{};
        winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem _moveToNewWindowMenuItem{};
        winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem _moveRightMenuItem{};
        winrt::Windows::UI::Xaml::Controls::MenuFlyoutItem _moveLeftMenuItem{};
        winrt::TerminalApp::ShortcutActionDispatch _dispatch;
        Microsoft::Terminal::Settings::Model::IActionMapView _actionMap{ nullptr };
        winrt::hstring _keyChord{};

        winrt::Microsoft::Terminal::Settings::Model::ThemeColor _themeColor{ nullptr };
        winrt::Microsoft::Terminal::Settings::Model::ThemeColor _unfocusedThemeColor{ nullptr };
        til::color _tabRowColor;

        Microsoft::Terminal::Settings::Model::TabCloseButtonVisibility _closeButtonVisibility{ Microsoft::Terminal::Settings::Model::TabCloseButtonVisibility::Always };

        std::shared_ptr<Pane> _rootPane{ nullptr };
        std::shared_ptr<Pane> _activePane{ nullptr };
        std::shared_ptr<Pane> _zoomedPane{ nullptr };

        Windows::UI::Xaml::Controls::MenuFlyoutItem _closePaneMenuItem;
        Windows::UI::Xaml::Controls::MenuFlyoutItem _restartConnectionMenuItem;

        winrt::Microsoft::Terminal::Settings::Model::IconStyle _lastIconStyle;
        winrt::hstring _lastIconPath{};
        std::optional<winrt::Windows::UI::Color> _runtimeTabColor{};
        winrt::TerminalApp::TabHeaderControl _headerControl{};
        winrt::TerminalApp::TerminalTabStatus _tabStatus{};

        winrt::TerminalApp::ColorPickupFlyout _tabColorPickup{ nullptr };
        winrt::event_token _colorSelectedToken;
        winrt::event_token _colorClearedToken;
        winrt::event_token _pickerClosedToken;

        struct ContentEventTokens
        {
            winrt::TerminalApp::IPaneContent::BellRequested_revoker BellRequested;
            winrt::TerminalApp::IPaneContent::TitleChanged_revoker TitleChanged;
            winrt::TerminalApp::IPaneContent::TabColorChanged_revoker TabColorChanged;
            winrt::TerminalApp::IPaneContent::TaskbarProgressChanged_revoker TaskbarProgressChanged;
            winrt::TerminalApp::IPaneContent::ConnectionStateChanged_revoker ConnectionStateChanged;
            winrt::TerminalApp::IPaneContent::ReadOnlyChanged_revoker ReadOnlyChanged;
            winrt::TerminalApp::IPaneContent::FocusRequested_revoker FocusRequested;

            // These events literally only apply if the content is a TermControl.
            winrt::Microsoft::Terminal::Control::TermControl::KeySent_revoker KeySent;
            winrt::Microsoft::Terminal::Control::TermControl::CharSent_revoker CharSent;
            winrt::Microsoft::Terminal::Control::TermControl::StringSent_revoker StringSent;

            winrt::TerminalApp::TerminalPaneContent::RestartTerminalRequested_revoker RestartTerminalRequested;
        };
        std::unordered_map<uint32_t, ContentEventTokens> _contentEvents;

        winrt::event_token _rootClosedToken{};

        std::vector<uint32_t> _mruPanes;
        uint32_t _nextPaneId{ 0 };

        bool _receivedKeyDown{ false };
        bool _iconHidden{ false };
        bool _changingActivePane{ false };

        winrt::hstring _runtimeTabText{};
        bool _inRename{ false };
        winrt::Windows::UI::Xaml::Controls::TextBox::LayoutUpdated_revoker _tabRenameBoxLayoutUpdatedRevoker;

        void _Setup();

        SafeDispatcherTimer _bellIndicatorTimer;
        void _BellIndicatorTimerTick(const Windows::Foundation::IInspectable& sender, const Windows::Foundation::IInspectable& e);

        void _UpdateHeaderControlMaxWidth();

        void _CreateContextMenu();
        winrt::hstring _CreateToolTipTitle();

        void _DetachEventHandlersFromContent(const uint32_t paneId);
        void _AttachEventHandlersToContent(const uint32_t paneId, const winrt::TerminalApp::IPaneContent& content);
        void _AttachEventHandlersToPane(std::shared_ptr<Pane> pane);

        void _UpdateActivePane(std::shared_ptr<Pane> pane);

        winrt::hstring _GetActiveTitle() const;

        void _RecalculateAndApplyReadOnly();

        void _UpdateProgressState();

        void _UpdateConnectionClosedState();
        void _RestartActivePaneConnection();

        void _DuplicateTab();

        winrt::Windows::UI::Xaml::Media::Brush _BackgroundBrush();

        void _MakeTabViewItem();

        void _AppendMoveMenuItems(winrt::Windows::UI::Xaml::Controls::MenuFlyout flyout);
        winrt::Windows::UI::Xaml::Controls::MenuFlyoutSubItem _AppendCloseMenuItems(winrt::Windows::UI::Xaml::Controls::MenuFlyout flyout);
        void _EnableMenuItems();
        void _UpdateSwitchToTabKeyChord();
        void _UpdateToolTip();

        void _RecalculateAndApplyTabColor();
        void _ApplyTabColorOnUIThread(const winrt::Windows::UI::Color& color);
        void _ClearTabBackgroundColor();
        void _RefreshVisualState();

        bool _focused() const noexcept;
        void _updateIsClosable();

        void _addBroadcastHandlers(const winrt::Microsoft::Terminal::Control::TermControl& control, ContentEventTokens& events);

        void _chooseColorClicked(const winrt::Windows::Foundation::IInspectable& sender, const winrt::Windows::UI::Xaml::RoutedEventArgs& e);
        void _renameTabClicked(const winrt::Windows::Foundation::IInspectable& sender, const winrt::Windows::UI::Xaml::RoutedEventArgs& e);
        void _duplicateTabClicked(const winrt::Windows::Foundation::IInspectable& sender, const winrt::Windows::UI::Xaml::RoutedEventArgs& e);
        void _splitTabClicked(const winrt::Windows::Foundation::IInspectable& sender, const winrt::Windows::UI::Xaml::RoutedEventArgs& e);
        void _closePaneClicked(const winrt::Windows::Foundation::IInspectable& sender, const winrt::Windows::UI::Xaml::RoutedEventArgs& e);
        void _exportTextClicked(const winrt::Windows::Foundation::IInspectable& sender, const winrt::Windows::UI::Xaml::RoutedEventArgs& e);
        void _findClicked(const winrt::Windows::Foundation::IInspectable& sender, const winrt::Windows::UI::Xaml::RoutedEventArgs& e);

        void _bubbleRestartTerminalRequested(TerminalApp::TerminalPaneContent sender, const winrt::Windows::Foundation::IInspectable& args);

        friend class ::TerminalAppLocalTests::TabTests;
    };
}

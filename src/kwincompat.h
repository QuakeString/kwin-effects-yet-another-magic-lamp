/*
 * Copyright (C) 2026 SLINIcraftet204
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include <effect/effecthandler.h>

#if __has_include(<scene/scene.h>)
// Defines KWin::RenderView, which carries the timestamp of the frame that is
// being prepared. KWin 6.7 dropped the presentation time from the pre-paint
// hooks, so this is where animations get their clock from now.
#include <scene/scene.h>
#define KWINCOMPAT_HAVE_RENDER_VIEW 1
#endif

#include <chrono>
#include <type_traits>

namespace KWinCompat {

template <typename EffectsHandler>
QRectF fullScreenArea(EffectsHandler* handler, KWin::LogicalOutput* screen)
{
    if constexpr (requires(EffectsHandler* h, KWin::LogicalOutput* s, KWin::VirtualDesktop* desktop) {
                      h->clientArea(KWin::FullScreenArea, s, desktop);
                  }) {
        return handler->clientArea(KWin::FullScreenArea, screen, handler->currentDesktop());
    } else {
        return handler->clientArea(KWin::FullScreenArea, screen);
    }
}

template <typename T>
std::chrono::milliseconds toMilliseconds(T value)
{
    using Value = std::remove_cvref_t<T>;

    if constexpr (std::is_integral_v<Value>) {
        return std::chrono::milliseconds(value);
    } else {
        return std::chrono::duration_cast<std::chrono::milliseconds>(value);
    }
}

inline std::chrono::milliseconds monotonicTimestamp()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch());
}

// The timestamp animations should be advanced to. Prefer the target presentation
// time of the frame being prepared, which is what KWin passed to prePaintScreen()
// before 6.7 and what the built-in effects still animate against; advancing on
// wall-clock time instead makes the animation jitter when a frame misses vblank.
template <typename RenderView>
std::chrono::milliseconds presentTime([[maybe_unused]] RenderView* view)
{
#ifdef KWINCOMPAT_HAVE_RENDER_VIEW
    if constexpr (requires { view->nextPresentationTimestamp(); }) {
        if (view) {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                view->nextPresentationTimestamp());
        }
    }
#endif
    return monotonicTimestamp();
}

template <typename ScreenPrePaintData>
std::chrono::milliseconds presentTime(const ScreenPrePaintData& data)
{
    if constexpr (requires { data.view; }) {
        return presentTime(data.view);
    } else {
        return monotonicTimestamp();
    }
}

template <typename EffectsHandler>
void prePaintScreen(EffectsHandler* handler, KWin::ScreenPrePaintData& data, std::chrono::milliseconds presentTime)
{
    if constexpr (requires(EffectsHandler* h) { h->prePaintScreen(data, presentTime); }) {
        handler->prePaintScreen(data, presentTime);
    } else {
        handler->prePaintScreen(data);
    }
}

template <typename EffectsHandler>
void prePaintWindow(EffectsHandler* handler, KWin::RenderView* view, KWin::EffectWindow* window,
    KWin::WindowPrePaintData& data, std::chrono::milliseconds presentTime)
{
    if constexpr (requires(EffectsHandler* h) { h->prePaintWindow(view, window, data, presentTime); }) {
        handler->prePaintWindow(view, window, data, presentTime);
    } else {
        handler->prePaintWindow(view, window, data);
    }
}

}

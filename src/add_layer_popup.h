#pragma once

#include "core/project.h"
#include "core/layer.h"
#include "core/circle_layer.h"
#include "imgui.h"
#include <memory>

namespace AddLayerPopup {

    
    inline void Render(MGE::Scene& scene, float playheadTime) {
        
        
        ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_FirstUseEver);

        if (ImGui::BeginPopup("AddLayerMenuPopup")) {
            
            if (ImGui::BeginTabBar("AddLayerTabs")) {
                
                
                if (ImGui::BeginTabItem("Shape")) {
                    if (ImGui::Selectable("Rectangle")) {
                        scene.addLayer(std::make_unique<MGE::Layer>("Rectangle", MGE::Time{(double)playheadTime}, MGE::Time{5.0}));
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::Selectable("Circle")) {
                        scene.addLayer(std::make_unique<MGE::CircleLayer>("Circle", MGE::Time{(double)playheadTime}, MGE::Time{5.0}, 50.0f));
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::Selectable("Triangle")) {
                        scene.addLayer(std::make_unique<MGE::Layer>("Triangle", MGE::Time{(double)playheadTime}, MGE::Time{5.0}));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndTabItem();
                }

                
                if (ImGui::BeginTabItem("Video")) {
                    ImGui::TextDisabled("Available Media:");
                    ImGui::Separator();
                    if (ImGui::Selectable("Gameplay_Footage.mp4")) {
                        scene.addLayer(std::make_unique<MGE::Layer>("Gameplay_Footage.mp4", MGE::Time{(double)playheadTime}, MGE::Time{15.0}));
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::Selectable("Intro_Cinematic.mov")) {
                        scene.addLayer(std::make_unique<MGE::Layer>("Intro_Cinematic.mov", MGE::Time{(double)playheadTime}, MGE::Time{8.5}));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndTabItem();
                }

                
                if (ImGui::BeginTabItem("Audio")) {
                    ImGui::TextDisabled("Available Audio Tracks:");
                    ImGui::Separator();
                    if (ImGui::Selectable("Background_Music.mp3")) {
                        scene.addLayer(std::make_unique<MGE::Layer>("Background_Music.mp3", MGE::Time{(double)playheadTime}, MGE::Time{60.0}));
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::Selectable("Voice_Over_01.wav")) {
                        scene.addLayer(std::make_unique<MGE::Layer>("Voice_Over_01.wav", MGE::Time{(double)playheadTime}, MGE::Time{12.0}));
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::Selectable("Sound_Effect_Boom.wav")) {
                        scene.addLayer(std::make_unique<MGE::Layer>("Sound_Effect_Boom.wav", MGE::Time{(double)playheadTime}, MGE::Time{2.0}));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndTabItem();
                }

                
                if (ImGui::BeginTabItem("Image")) {
                    ImGui::TextDisabled("Available Images:");
                    ImGui::Separator();
                    if (ImGui::Selectable("Company_Logo.png")) {
                        scene.addLayer(std::make_unique<MGE::Layer>("Company_Logo.png", MGE::Time{(double)playheadTime}, MGE::Time{5.0}));
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::Selectable("Watermark.png")) {
                        scene.addLayer(std::make_unique<MGE::Layer>("Watermark.png", MGE::Time{(double)playheadTime}, MGE::Time{99.0}));
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
            ImGui::EndPopup();
        }
    }
}
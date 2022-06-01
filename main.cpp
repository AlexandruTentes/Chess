//#define GLFW_INCLUDE_NONE
//#include "include/glfw3.h"
//#include "include/glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <fstream>
#include <conio.h>
#include <unordered_map>
#include <stdlib.h>
#include <string>
#include <vector>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lib/stb_image.h"
#include "ChessFunctions.h"
#include "ChessEngine.h"

//GENERATED_INPUT_IN
#include "Utility.h"
#include "Structure.h"
#include "UI.h"
//GENERATED_INPUT_OUT
//GENERATED_OUTPUT

using namespace std;

bool print_board = true;
bool running = true;
bool check_once = false;
bool is_in_check = false;
int cell_size = 100;
float piece_size = 0.85;
vector<tile*> valid_tiles;
tile* selected_tile = nullptr;
unordered_map<string, Texture> texture_map;
//GEN_CLASS_DoublyLinkedList<tile> chess;


void draw_ui(ImVec4 & clear_color)
{
    ImVec4 black(0.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 white(1.0f, 1.0f, 1.0f, 1.0f);
    bool change_player = false;

    if (print_board)
    {
        if (!check_once)
        {
            tile* next = getOwnPiece("King");

            is_in_check = isKingInCheck(next);

            cout << "King in check: " << is_in_check << "\n";
            check_once = true;
        }

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            bool is_tile_open = true;
            string name = "tile_" + to_string(i);
            int x = (i * cell_size) % (ROW * cell_size);
            int y = (i / ROW) * cell_size;
            bool piece_highlighted = false;

            ImGui::SetNextWindowPos(ImVec2(x, y));
            ImGui::SetNextWindowSize(ImVec2(cell_size, cell_size));

            ImGui::PushStyleColor(ImGuiCol_WindowBg, ((((x + y) / cell_size) 
                % ROW) % 2 == 0 ? white : black));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin(name.c_str(), &is_tile_open,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoFocusOnAppearing | 
                ImGuiWindowFlags_NoBringToFrontOnFocus | 
                ImGuiWindowFlags_NoScrollbar);
            ImGui::PopStyleVar();

            //Centering the image according to it s picked size
            ImVec2 img_size(cell_size * piece_size,
                cell_size * piece_size);
            int x_img = (ImGui::GetWindowSize()[0] -
                img_size[0]) * 0.5;
            int y_img = (ImGui::GetWindowSize()[1] -
                img_size[1]) * 0.5;

            tile* node = search_pos(i);

            if (node != nullptr && node->is_valid_tile)
            {
                Texture& valid_moves_tex = texture_map["valid_move"];
                ImGui::SetCursorPos(ImVec2(x_img, y_img));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                    ImVec2(0, 0));
                ImGui::PushStyleColor(ImGuiCol_Button,
                    ImVec4(0.f, 0.f, 0.f, 0.f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                    ImVec4(0.625f, 0.625f, 0.625f, 1.f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                    ImVec4(0.875f, 0.875f, 0.875f, 0.4f));
                if (ImGui::ImageButton((void*)(intptr_t)valid_moves_tex.my_image_texture,
                    img_size, ImVec2(0, 0), ImVec2(1, 1), 0,
                    ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
                {
                    
                    if (selected_tile != nullptr)
                    {
                        if (is_in_check)
                        {
                            is_in_check = isKingInCheckNextMove(
                                selected_tile, node, change_player);
                        }
                        else
                        {
                            if (selected_tile->piece == "King" &&
                                abs(selected_tile->x - node->x) != 1)
                            {
                                if (!isKingInCheckNextMove(selected_tile, node, change_player))
                                {
                                    if (left(node) != nullptr && left(node)->piece == "Rook")
                                    {
                                        regularMove(left(node), right(node));
                                    }
                                    else if (right(node) != nullptr && right(node)->piece == "Rook")
                                    {
                                        regularMove(right(node), left(node));
                                    }
                                    else if (left(left(node)) != nullptr &&
                                        left(left(node))->piece == "Rook")
                                    {
                                        regularMove(left(left(node)), right(node));
                                    }
                                    else if (right(right(node)) != nullptr &&
                                        right(right(node))->piece == "Rook")
                                    {
                                        regularMove(right(right(node)), left(node));
                                    }
                                }
                            }
                            else if (selected_tile->piece == "Pawn" &&
                                selected_tile->x != node->x &&
                                node->piece == "")
                            {
                                tile* enpassant_tile = search_pos(i + 8);
                                regularMove(selected_tile, node);
                                enpassant_tile->clear();
                            }
                            else
                            {
                                isKingInCheckNextMove(selected_tile, node, change_player);
                            }
                        }
                    }
                    check_once = false;

                    if (change_player)
                    {
                        rotate_list();

                        for (auto& tile : valid_tiles)
                            tile->is_valid_tile = false;

                        selected_tile = nullptr;
                    }
                }
                ImGui::PopStyleVar();
                ImGui::PopStyleColor(3);
            }

            if (node != nullptr && node->tex.piece != "")
            {
                bool clicked = false;
                if (node->piece == "King" && is_in_check && node->isOwnPiece)
                {
                    Texture& tex = texture_map["check"];
                    ImGui::SetCursorPos(ImVec2(x_img, y_img));
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2(0, 0));
                    ImGui::PushStyleColor(ImGuiCol_Button,
                        ImVec4(0.f, 0.f, 0.f, 0.f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                        ImVec4(0.625f, 0.625f, 0.625f, 0.f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                        ImVec4(0.875f, 0.875f, 0.875f, 0.f));
                    if (ImGui::ImageButton((void*)(intptr_t)tex.my_image_texture,
                        img_size, ImVec2(0, 0), ImVec2(1, 1), 0,
                        ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
                        clicked = true;
                    ImGui::PopStyleVar();
                    ImGui::PopStyleColor(3);
                }

                Texture& tex = texture_map[node->tex.piece];
                ImGui::SetCursorPos(ImVec2(x_img, y_img));
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, 
                    ImVec2(0, 0));
                ImGui::PushStyleColor(ImGuiCol_Button,
                    ImVec4(0.f, 0.f, 0.f, 0.f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                    ImVec4(0.625f, 0.625f, 0.625f, 1.f * 
                        (node->isOwnPiece == true)));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                    ImVec4(0.875f, 0.875f, 0.875f, 0.4f * 
                        (node->isOwnPiece == true)));
                if (ImGui::ImageButton((void*)(intptr_t)tex.my_image_texture,
                    img_size, ImVec2(0, 0), ImVec2(1, 1), 0, 
                    ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
                    clicked = true;
                ImGui::PopStyleVar();
                ImGui::PopStyleColor(3);

                if (clicked)
                {
                    if (node->isOwnPiece)
                    {
                        cout << "Piece " << node->tex.piece <<
                            " has been clicked!\n";

                        piece_highlighted = true;
                    }
                }
            }

            if (piece_highlighted)
            {
                piece_highlighted = false;

                for (auto& tile : valid_tiles)
                    tile->is_valid_tile = false;

                valid_tiles = callCorrectFunction(node->piece, node);
                cout << "Valid tiles: " << valid_tiles.size() << "\n";

                for (auto& tile : valid_tiles)
                    tile->is_valid_tile = true;

                selected_tile = node;
            }

            ImGui::PopStyleColor();
            ImGui::End();
        }
    }
}

void set_textures()
{
    Texture & pawn_black = texture_map["pawn_black"];
    bool tex_1 = LoadTextureFromFile("Images/pawn_black.jpg",
        &pawn_black.my_image_texture,
        &pawn_black.my_image_width,
        &pawn_black.my_image_height);

    Texture& knight_black = texture_map["knight_black"];
    bool tex_2 = LoadTextureFromFile("Images/knight_black.jpg",
        &knight_black.my_image_texture,
        &knight_black.my_image_width,
        &knight_black.my_image_height);

    Texture& rook_black = texture_map["rook_black"];
    bool tex_3 = LoadTextureFromFile("Images/rook_black.jpg",
        &rook_black.my_image_texture,
        &rook_black.my_image_width,
        &rook_black.my_image_height);

    Texture& bishop_black = texture_map["bishop_black"];
    bool tex_4 = LoadTextureFromFile("Images/bishop_black.jpg",
        &bishop_black.my_image_texture,
        &bishop_black.my_image_width,
        &bishop_black.my_image_height);

    Texture& queen_black = texture_map["queen_black"];
    bool tex_5 = LoadTextureFromFile("Images/queen_black.jpg",
        &queen_black.my_image_texture,
        &queen_black.my_image_width,
        &queen_black.my_image_height);

    Texture& king_black = texture_map["king_black"];
    bool tex_6 = LoadTextureFromFile("Images/king_black.jpg",
        &king_black.my_image_texture,
        &king_black.my_image_width,
        &king_black.my_image_height);

    //Whites

    Texture& pawn_white = texture_map["pawn_white"];
    bool tex_7 = LoadTextureFromFile("Images/pawn_white.jpg",
        &pawn_white.my_image_texture,
        &pawn_white.my_image_width,
        &pawn_white.my_image_height);

    Texture& knight_white = texture_map["knight_white"];
    bool tex_8 = LoadTextureFromFile("Images/knight_white.jpg",
        &knight_white.my_image_texture,
        &knight_white.my_image_width,
        &knight_white.my_image_height);

    Texture& rook_white = texture_map["rook_white"];
    bool tex_9 = LoadTextureFromFile("Images/rook_white.jpg",
        &rook_white.my_image_texture,
        &rook_white.my_image_width,
        &rook_white.my_image_height);

    Texture& bishop_white = texture_map["bishop_white"];
    bool tex_10 = LoadTextureFromFile("Images/bishop_white.jpg",
        &bishop_white.my_image_texture,
        &bishop_white.my_image_width,
        &bishop_white.my_image_height);

    Texture& queen_white = texture_map["queen_white"];
    bool tex_11 = LoadTextureFromFile("Images/queen_white.jpg",
        &queen_white.my_image_texture,
        &queen_white.my_image_width,
        &queen_white.my_image_height);

    Texture& king_white = texture_map["king_white"];
    bool tex_12 = LoadTextureFromFile("Images/king_white.jpg",
        &king_white.my_image_texture,
        &king_white.my_image_width,
        &king_white.my_image_height);

    Texture& valid_move = texture_map["valid_move"];
    bool tex_13 = LoadTextureFromFile("Images/valid_move.jpg",
        &valid_move.my_image_texture,
        &valid_move.my_image_width,
        &valid_move.my_image_height);

    Texture& check = texture_map["check"];
    bool tex_14 = LoadTextureFromFile("Images/check.jpg",
        &check.my_image_texture,
        &check.my_image_width,
        &check.my_image_height);

    Texture& check_source = texture_map["check_source"];
    bool tex_15 = LoadTextureFromFile("Images/check_source.jpg",
        &check_source.my_image_texture,
        &check_source.my_image_width,
        &check_source.my_image_height);

    IM_ASSERT(tex_1); IM_ASSERT(tex_2); IM_ASSERT(tex_3);
    IM_ASSERT(tex_4); IM_ASSERT(tex_5); IM_ASSERT(tex_6);
    IM_ASSERT(tex_7); IM_ASSERT(tex_8); IM_ASSERT(tex_9);
    IM_ASSERT(tex_10); IM_ASSERT(tex_11); IM_ASSERT(tex_12);
    IM_ASSERT(tex_13); IM_ASSERT(tex_14); IM_ASSERT(tex_15);
}


/*MULTI_COMM
void populate_chessboard()
{
    
    // Top
    chess.add_back(tile(0, "rook_white"));
    chess.add_back(tile(1, "knight_white"));
    chess.add_back(tile(2, "bishop_white"));
    chess.add_back(tile(3, "king_white"));
    chess.add_back(tile(4, "queen_white"));
    chess.add_back(tile(5, "bishop_white"));
    chess.add_back(tile(6, "knight_white"));
    chess.add_back(tile(7, "rook_white"));

    for (int i = 8; i < 56; i++)
    {
        tile t;
        t.x = i;
        if (i < 16)
            t.tex.piece = "pawn_white";
        else if (i >= 48)
            t.tex.piece = "pawn_black";
        chess.add_back(t);
    }

    // Bottom
    chess.add_back(tile(56, "rook_black"));
    chess.add_back(tile(57, "knight_black"));
    chess.add_back(tile(58, "bishop_black"));
    chess.add_back(tile(59, "king_black"));
    chess.add_back(tile(60, "queen_black"));
    chess.add_back(tile(61, "bishop_black"));
    chess.add_back(tile(62, "knight_black"));
    chess.add_back(tile(63, "rook_black"));
}
MULTI_COMM*/

int main()
{
    print_board = true;
    cell_size = 100;
    piece_size = 0.85;

    GEN_CLASS_UI ui;
    ui.window_width = ROW * cell_size;
    ui.window_height = ROW * cell_size;

    ui.init();
    set_textures();

    makeBoard();
    populateBoard();

    //populate_chessboard();

    while (ui.running() && running)
    {
        ui.loop(draw_ui);
    }
}
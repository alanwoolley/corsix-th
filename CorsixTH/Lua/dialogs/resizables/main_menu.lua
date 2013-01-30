--[[ Copyright (c) 2010 Manuel "Roujin" Wolf

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. --]]

--! Class for main menu window.
class "UIMainMenu" (UIResizable)

local col_bg = {
  red = 154,
  green = 146,
  blue = 198,
}

function UIMainMenu:UIMainMenu(ui)
  self:UIResizable(ui, 280, 390, col_bg)
  
  local app = ui.app
  self.esc_closes = false
  self.modal_class = "main menu"
  self.on_top = true
  self:setDefaultPosition(0.5, 0.1)

  -- The main menu also shows the version number of the player's copy of the game.
  self.label_font = TheApp.gfx:loadFont("QData", "Font01V")
  self.version_number = TheApp:getVersion()
  
  -- individual buttons
  self.default_button_sound = "selectx.wav"
  self:addBevelPanel(20, 20, 240, 60, col_bg):setLabel(_S.main_menu.new_game):makeButton(0, 0, 240, 60, nil, self.buttonNewGame):setTooltip(_S.tooltip.main_menu.new_game)
  self:addBevelPanel(20, 85, 240, 60, col_bg):setLabel(_S.main_menu.custom_level):makeButton(0, 0, 240, 60, nil, self.buttonCustomGame):setTooltip(_S.tooltip.main_menu.custom_level)
  self:addBevelPanel(20, 150, 240, 60, col_bg):setLabel(_S.main_menu.load_game):makeButton(0, 0, 240, 60, nil, self.buttonLoadGame):setTooltip(_S.tooltip.main_menu.load_game)
  
  self:addBevelPanel(20, 215, 240, 60, col_bg):setLabel("Quick Load"):makeButton(0, 0, 240, 60, nil, self.buttonQuickLoad):setTooltip("Quick Load")
 
  self:addBevelPanel(20, 295, 240, 60, col_bg):setLabel(_S.main_menu.exit):makeButton(0, 0, 240, 60, nil, self.buttonExit):setTooltip(_S.tooltip.main_menu.exit)
end

function UIMainMenu:getSavedWindowPositionName()
  return "main_menu_group"
end

local label_y = { 27, 75, 123, 171, 231 }

function UIMainMenu:draw(canvas, x, y)
  UIResizable.draw(self, canvas, x, y)
  x, y = self.x + x, self.y + y

  -- Move the version string up a bit if also showing the savegame version.
  local ly = y + 375
  if TheApp.config.debug then
    self.label_font:draw(canvas, _S.main_menu.savegame_version .. TheApp.savegame_version, x + 5, ly, 270, 0, "right")
    ly = ly - 15
  end
  self.label_font:draw(canvas, _S.main_menu.version .. self.version_number, x + 5, ly, 270, 0, "right")
end

function UIMainMenu:buttonNewGame()
  local window = UINewGame(self.ui)
  self.ui:addWindow(window)
end

function UIMainMenu:buttonCustomGame()
  if TheApp.using_demo_files then
    self.ui:addWindow(UIInformation(self.ui, {_S.information.no_custom_game_in_demo}))
  else
    local window = UICustomGame(self.ui)
    self.ui:addWindow(window)
  end
end

function UIMainMenu:buttonLoadGame()
  showloaddialog()
end

function UIMainMenu:buttonQuickLoad()
  quickload()
end

function UIMainMenu:buttonOptions()
  local window = UIOptions(self.ui, "menu")
  self.ui:addWindow(window)
end

function UIMainMenu:buttonExit()
  self.ui.app:exit()
end

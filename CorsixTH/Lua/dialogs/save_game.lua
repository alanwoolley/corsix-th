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

dofile("dialogs/resizable")

--! Save Game Window
class "UISaveGame" (UIResizable)

local col_bg = {
  red = 154,
  green = 146,
  blue = 198,
}

local col_caption = {
  red = 174,
  green = 166,
  blue = 218,
}

local col_scrollbar = {
  red = 164,
  green = 156,
  blue = 208,
}

function UISaveGame:UISaveGame(ui)
  self:UIResizable(ui, 200, 280, col_bg)
  
  local app = ui.app
  self.modal_class = "saveload"
  self.esc_closes = true
  self.resizable = false
  self:setDefaultPosition(0.5, 0.25)
  
  -- Scan for savegames
  self.saves = self.ui.app:scanSavegames()
  
  -- Window Caption
  self:addBevelPanel(20, 10, 160, 20, col_caption):setLabel(_S.save_game_window.caption)
    .lowered = true
  
  -- Scrollbar
  local scrollbar_base = self:addBevelPanel(160, 40, 20, 8*17, col_bg)
  scrollbar_base.lowered = true
  self.scrollbar = scrollbar_base:makeScrollbar(col_scrollbar, --[[persistable:save_game_scrollbar_callback]] function()
    self:updateButtons()
  end, 1, math.max(#self.saves, 1), 8)
  
  local function save_button(num)
    return --[[persistable:save_game_button]] function(self)
      self:buttonSave(num)
    end
  end
  
  -- List of existing savegames
  self.savegame_panels = {}
  self.savegame_buttons = {}
  for num = 1, 8 do
    self.savegame_panels[num] = self:addBevelPanel(20, 40 + (num - 1) * 17, 130, 17, col_bg)
    self.savegame_buttons[num] = self.savegame_panels[num]:makeButton(0, 0, 130, 17, nil, save_button(num))
  end
  
  -- Textbox for entering new savegame name
  self.new_savegame_textbox = self:addBevelPanel(20, 190, 160, 17, col_bg):setLabel(_S.save_game_window.new_save_game):setTooltip(_S.tooltip.save_game_window.new_save_game)
    :makeTextbox(--[[persistable:save_game_new_savegame_textbox_confirm_callback]] function() self:confirmName() end,
    --[[persistable:save_game_new_savegame_textbox_abort_callback]] function() self:abortName() end)
  
  -- Back button
  self:addBevelPanel(20, 220, 160, 40, col_bg):setLabel(_S.save_game_window.back)
    :makeButton(0, 0, 160, 40, nil, self.buttonBack):setTooltip(_S.tooltip.save_game_window.back)
  
  self:updateButtons()
end

function UISaveGame:updateButtons()
  for num = 1, 8 do
    local panel = self.savegame_panels[num]
    local button = self.savegame_buttons[num]
    local filename = self.saves[num + self.scrollbar.value - 1]
    if filename then
      panel:setLabel(filename)
      panel:setTooltip(_S.tooltip.save_game_window.save_game:format(filename))
      button:enable(true)
    else
      panel:setLabel()
      panel:setTooltip()
      button:enable(false)
    end
  end
end

--! Function called when textbox is aborted (e.g. by pressing escape)
function UISaveGame:abortName()
  self.new_savegame_textbox.text = ""
  self.new_savegame_textbox.panel:setLabel(_S.save_game_window.new_save_game)
end

--! Function called when textbox is confirmed (e.g. by pressing enter)
function UISaveGame:confirmName()
  local filename = self.new_savegame_textbox.text
  if filename == "" then
    self:abortName()
    return
  end
  self:trySave(filename)
end

--! Function called by clicking button of existing save #num
function UISaveGame:buttonSave(num)
  local filename = self.saves[num + self.scrollbar.value - 1]
  self:trySave(filename)
end

--! Try to save the game with given filename; if already exists, create confirmation window first.
function UISaveGame:trySave(filename)
  local found = false
  for _, save in ipairs(self.saves) do
    if save:lower() == filename:lower() then
      found = true
      break
    end
  end
  if found then
    self.ui:addWindow(UIConfirmDialog(self.ui, _S.confirmation.overwrite_save, --[[persistable:save_game_confirmation]] function() self:doSave(filename) end))
  else
    self:doSave(filename)
  end
end

--! Actually do save the game with given filename.
function UISaveGame:doSave(filename)
  filename = filename .. ".sav"
  local ui = self.ui
  self:close()
  
  local status, err = pcall(SaveGameFile, filename)
  if not status then
    err = _S.errors.save_prefix .. err
    print(err)
    ui:addWindow(UIInformation(ui, {err}))
  end
end

function UISaveGame:buttonBack()
  if self.mode == "menu" then
    self.ui:addWindow(UIMainMenu(self.ui))
  end
  self:close()
end

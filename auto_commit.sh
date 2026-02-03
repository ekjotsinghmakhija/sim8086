#!/bin/bash

# --- 1. SETUP SAFETY (Create .gitignore) ---
# We do this first so we don't accidentally commit the 'build' folders or binaries
echo "Generating .gitignore..."
cat > .gitignore <<EOL
build/
build_linux/
build_mac/
build_win/
*.o
*.obj
*.exe
*.app
.DS_Store
.vscode/
Sim8086
Emu8086
EOL

# --- 2. RESET STAGING AREA ---
# Unstages all files so we can add them one by one
git init
git reset

# --- 3. COMMIT LOOP ---
# This command lists all untracked files while respecting the .gitignore
echo "Starting individual file commits..."

# We use a temporary file list to handle spaces in filenames correctly
git ls-files --others --exclude-standard | while read file; do
    git add "$file"
    git commit -m "Add $(basename "$file")"
    echo "Committed: $file"
done

# --- 4. SETUP REMOTE & PUSH ---
echo "--------------------------------"
echo "All files committed!"
echo "Pushing to GitHub..."

# Rename branch to main
git branch -M main

# Add origin (Errors suppressed in case it already exists)
git remote add origin git@github.com:ekjotsinghmakhija/sim8086.git 2>/dev/null

# Push
git push -u origin main

// DOM Elements
const canvas = document.getElementById('canvas');
const generatedCode = document.getElementById('generatedCode');
const programOutput = document.getElementById('programOutput');
const runBtn = document.getElementById('runBtn');
const clearBtn = document.getElementById('clearBtn');
const exportBtn = document.getElementById('exportBtn');
const languageSelect = document.getElementById('languageSelect');
const syntaxSelect = document.getElementById('syntaxSelect');

// Drag and Drop from Palette
document.querySelectorAll('.palette .block').forEach(block => {
    block.addEventListener('dragstart', (e) => {
        e.dataTransfer.setData('blockType', block.dataset.type);
        e.dataTransfer.effectAllowed = 'copy';
    });
});

// Canvas drop zone
canvas.addEventListener('dragover', (e) => {
    e.preventDefault();
    e.dataTransfer.dropEffect = 'copy';
    canvas.classList.add('drag-over');
});

canvas.addEventListener('dragleave', () => {
    canvas.classList.remove('drag-over');
});

canvas.addEventListener('drop', (e) => {
    e.preventDefault();
    canvas.classList.remove('drag-over');
    
    const blockType = e.dataTransfer.getData('blockType');
    if (blockType) {
        addBlock(blockType);
    }
});

// Add block to canvas
function addBlock(type) {
    const block = new Block(type);
    window.blocks.push(block);
    console.log('Added block:', type, 'Fields:', block.fields);
    renderCanvas();
    updateCodePreview();
    
    // Remove empty state
    const emptyState = canvas.querySelector('.empty-state');
    if (emptyState) emptyState.remove();
}

// Render all blocks on canvas
function renderCanvas() {
    // Keep existing blocks, just update
    const existingBlocks = canvas.querySelectorAll('.canvas-block');
    existingBlocks.forEach(el => el.remove());
    
    window.blocks.forEach(block => {
        const blockHTML = block.generateHTML();
        console.log('Rendering block HTML:', blockHTML.substring(0, 200));
        canvas.insertAdjacentHTML('beforeend', blockHTML);
    });
}

// Update block field value
window.updateBlockField = function(blockId, fieldName, value) {
    const block = window.blocks.find(b => b.id === blockId);
    if (block) {
        block.fields[fieldName] = value;
        updateCodePreview();
    }
};

// Delete block
window.deleteBlock = function(blockId) {
    window.blocks = window.blocks.filter(b => b.id !== blockId);
    renderCanvas();
    updateCodePreview();
    
    // Show empty state if no blocks
    if (window.blocks.length === 0) {
        canvas.innerHTML = '<div class="empty-state">Drag blocks here to start coding!</div>';
    }
};

// Update code preview
function updateCodePreview() {
    let code = '';
    window.blocks.forEach(block => {
        code += block.generateCode(0) + '\n';
    });
    
    if (code.trim() === '') {
        code = '# Drag blocks to generate code';
    }
    
    generatedCode.textContent = code;
}

// Clear canvas
clearBtn.addEventListener('click', () => {
    if (confirm('Clear all blocks?')) {
        window.blocks = [];
        canvas.innerHTML = '<div class="empty-state">Drag blocks here to start coding!</div>';
        updateCodePreview();
    }
});

// Export code
exportBtn.addEventListener('click', () => {
    const code = generatedCode.textContent;
    const language = languageSelect.value;
    const syntax = syntaxSelect.value;
    
    // Add language/syntax directives
    const fullCode = `# Language: ${language}\n# Syntax: ${syntax}\n\n${code}`;
    
    // Create download
    const blob = new Blob([fullCode], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'program.mlp';
    a.click();
    URL.revokeObjectURL(url);
});

// Run program
runBtn.addEventListener('click', async () => {
    const code = generatedCode.textContent;
    if (code === '# Drag blocks to generate code') {
        alert('Add some blocks first!');
        return;
    }
    
    programOutput.textContent = '⏳ Compiling and running...\n';
    switchToOutputTab();
    
    try {
        // Save code to temp file
        const language = languageSelect.value;
        const syntax = syntaxSelect.value;
        const fullCode = `# Language: ${language}\n# Syntax: ${syntax}\n\n${code}`;
        
        // Call backend to compile and run
        const response = await fetch('/api/run', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ code: fullCode })
        });
        
        const result = await response.json();
        
        if (result.success) {
            programOutput.textContent = '✅ Output:\n\n' + result.output;
        } else {
            programOutput.textContent = '❌ Error:\n\n' + result.error;
        }
    } catch (error) {
        programOutput.textContent = '❌ Failed to run:\n\n' + error.message;
    }
});

// Tab switching
document.querySelectorAll('.tab').forEach(tab => {
    tab.addEventListener('click', () => {
        const targetTab = tab.dataset.tab;
        
        // Update active tab
        document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
        tab.classList.add('active');
        
        // Show corresponding content
        document.querySelectorAll('.tab-content').forEach(content => {
            content.classList.add('hidden');
        });
        
        if (targetTab === 'code') {
            document.getElementById('codeTab').classList.remove('hidden');
        } else {
            document.getElementById('outputTab').classList.remove('hidden');
        }
    });
});

function switchToOutputTab() {
    document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
    document.querySelector('[data-tab="output"]').classList.add('active');
    document.getElementById('codeTab').classList.add('hidden');
    document.getElementById('outputTab').classList.remove('hidden');
}

// Initialize
updateCodePreview();

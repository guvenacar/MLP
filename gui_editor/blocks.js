// Block definitions with MLP code templates
const blockTemplates = {
    'for': {
        name: 'For Loop',
        color: 'for-block',
        fields: [
            { name: 'variable', label: 'Variable:', value: 'i' },
            { name: 'start', label: 'Start:', value: '0' },
            { name: 'end', label: 'End:', value: '10' },
            { name: 'step', label: 'Step:', value: '', placeholder: 'optional' }
        ],
        hasBody: true,
        generate: (fields) => {
            const step = fields.step ? ` step ${fields.step}` : '';
            return `for ${fields.variable} = ${fields.start} to ${fields.end}${step}`;
        }
    },
    'while': {
        name: 'While Loop',
        color: 'while-block',
        fields: [
            { name: 'condition', label: 'Condition:', value: 'x < 10' }
        ],
        hasBody: true,
        generate: (fields) => `while ${fields.condition}`
    },
    'if': {
        name: 'If Statement',
        color: 'if-block',
        fields: [
            { name: 'condition', label: 'Condition:', value: 'x > 0' }
        ],
        hasBody: true,
        hasElse: true,
        generate: (fields) => `if ${fields.condition} then`
    },
    'variable': {
        name: 'Variable',
        color: 'var-block',
        fields: [
            { name: 'type', label: 'Type:', value: 'SAYISAL', options: ['SAYISAL', 'METIN', 'BOOL'] },
            { name: 'name', label: 'Name:', value: 'x' },
            { name: 'value', label: 'Value:', value: '0' }
        ],
        generate: (fields) => `${fields.type} ${fields.name} = ${fields.value}`
    },
    'assign': {
        name: 'Assignment',
        color: 'var-block',
        fields: [
            { name: 'variable', label: 'Variable:', value: 'x' },
            { name: 'expression', label: 'Expression:', value: 'x + 1' }
        ],
        generate: (fields) => `${fields.variable} = ${fields.expression}`
    },
    'print': {
        name: 'Print',
        color: 'io-block',
        fields: [
            { name: 'value', label: 'Value:', value: 'x' }
        ],
        generate: (fields) => `print(${fields.value})`
    },
    'function': {
        name: 'Function',
        color: 'func-block',
        fields: [
            { name: 'name', label: 'Name:', value: 'myFunc' },
            { name: 'params', label: 'Parameters:', value: '', placeholder: 'a, b' }
        ],
        hasBody: true,
        generate: (fields) => {
            const params = fields.params || '';
            return `işleç ${fields.name}(${params})`;
        }
    },
    'return': {
        name: 'Return',
        color: 'func-block',
        fields: [
            { name: 'value', label: 'Value:', value: 'result' }
        ],
        generate: (fields) => `return ${fields.value}`
    }
};

// Block class to manage individual blocks on canvas
class Block {
    constructor(type, id = null) {
        this.type = type;
        this.id = id || `block_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
        this.template = blockTemplates[type];
        this.fields = {};
        this.children = []; // For blocks inside loops/conditions
        
        // Initialize fields with default values
        this.template.fields.forEach(field => {
            this.fields[field.name] = field.value;
        });
    }

    generateHTML() {
        const template = this.template;
        let html = `
            <div class="canvas-block" data-id="${this.id}" data-type="${this.type}">
                <div class="block-header ${template.color}">
                    <span>${template.name}</span>
                    <button class="delete-btn" onclick="deleteBlock('${this.id}')">✕</button>
                </div>
                <div class="block-body">
        `;

        // Generate input fields
        template.fields.forEach(field => {
            if (field.options) {
                // Dropdown
                html += `
                    <label>${field.label}</label>
                    <select data-field="${field.name}" onchange="updateBlockField('${this.id}', '${field.name}', this.value)">
                        ${field.options.map(opt => 
                            `<option value="${opt}" ${opt === this.fields[field.name] ? 'selected' : ''}>${opt}</option>`
                        ).join('')}
                    </select>
                `;
            } else {
                // Text input
                html += `
                    <label>${field.label}</label>
                    <input type="text" 
                           value="${this.fields[field.name]}" 
                           placeholder="${field.placeholder || ''}"
                           data-field="${field.name}"
                           oninput="updateBlockField('${this.id}', '${field.name}', this.value)">
                `;
            }
        });

        html += `</div>`;
        
        // Body for loops/conditions
        if (template.hasBody) {
            html += `
                <div class="block-body" style="background: #f9f9f9; min-height: 40px; border-top: 2px solid #ddd;">
                    <div class="nested-drop-zone" data-parent="${this.id}">
                        <em style="color: #aaa;">Drop blocks inside...</em>
                    </div>
                </div>
            `;
        }

        html += `</div>`;
        return html;
    }

    generateCode(indent = 0) {
        const template = this.template;
        const indentStr = '    '.repeat(indent);
        let code = indentStr + template.generate(this.fields);

        if (template.hasBody) {
            code += '\n';
            this.children.forEach(child => {
                code += child.generateCode(indent + 1) + '\n';
            });
            code += indentStr + 'end';
        }

        return code;
    }
}

// Global state
window.blocks = [];
window.currentDragBlock = null;

#include <array>
#include <iostream>
#include <string>

class ConcreteComponentA;
class ConcreteComponentB;

// visitor interface declare a set of methods that corresponds to
// each concrete component classes.
// The problem of this pattern is you need to have a interface
// for each concrete class you gonna work with.
class Visitor {
public:
  virtual void visit_concreteA(const ConcreteComponentA *element) const = 0;
  virtual void visit_concreteB(const ConcreteComponentB *element) const = 0;
};

class Component {
public:
  virtual ~Component() {}
  virtual void accept(Visitor *visitor) const = 0;
};

class ConcreteComponentA : public Component {
public:
  void accept(Visitor *visitor) const override {
    visitor->visit_concreteA(this);
  }

  std::string exclusive_method_of_A() const { return "A"; }
};

class ConcreteComponentB : public Component {
public:
  void accept(Visitor *visitor) const override {
    visitor->visit_concreteB(this);
  }

  std::string exclusive_method_of_B() const { return "A"; }
};

// define two concrete visitors with different implementation
// of visiti
class ConcreteVisitor1 : public Visitor {
public:
  void visit_concreteA(const ConcreteComponentA *element) const override {
    std::cout << element->exclusive_method_of_A() << " + visitor1" << std::endl;
  }

  void visit_concreteB(const ConcreteComponentB *element) const override {
    std::cout << element->exclusive_method_of_B() << " + visitor1" << std::endl;
  }
};

class ConcreteVisitor2 : public Visitor {
public:
  void visit_concreteA(const ConcreteComponentA *element) const override {
    std::cout << element->exclusive_method_of_A() << " + visitor2" << std::endl;
  }

  void visit_concreteB(const ConcreteComponentB *element) const override {
    std::cout << element->exclusive_method_of_B() << " + visitor2" << std::endl;
  }
};
;

// client can run visitor operation over components.
// basically it traverse the array and call the right function
// based on what concrete type it meets.
void client(std::array<const Component *, 2> components, Visitor *visitor) {

  for (const Component *comp : components) {
    comp->accept(visitor);
  }
}

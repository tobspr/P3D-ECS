## Open Questions

These are the questions about the implementation of the ECS I haven't found an
answer to yet.

- Is it the job of transform component to detect recursive parent<->child relations?

- When removing an entity, should it also remove any children, or should an additional
  method like `entity->remove_recursive()` exist?

- How are undo-steps in the Editor implemented, for more complex actions like
  deleting entities? Maybe store all deleted entities as serialized elements
  and re-import them?

- How are changes to entities tracked? E.g. a server side script sets a new position
  on an entity - the client has to get notified

    - Related: How to efficiently generate entity deltas. I.e. to generate a delta to
      move from GameState_0 to GameState_1

